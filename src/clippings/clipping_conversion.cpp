/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include <Fl/Fl.H>
#include "src/clippings/clipping_conversion.h"

namespace vcutter {


ClippingConversion::ClippingConversion(
    std::shared_ptr<ProgressHandler> prog_handler,
    std::shared_ptr<Clipping> clipping,
    uint32_t max_memory
) {
    max_memory_ = max_memory;
    clipping_ = clipping;
    prog_handler_ = prog_handler;
    current_buffer_.store(NULL);
    current_position_.store(0);
    max_position_ = 1;
}

bool ClippingConversion::convert(
    const char *codec,
    const char *path,
    uint32_t bitrate,
    double fps,
    bool from_start,
    bool append_reverse,
    uint8_t transition_frames
) {
    current_position_.store(0);
    max_position_ = clipping_->duration_frames() * (append_reverse ? 2 : 1);
    allocate_buffers(from_start, append_reverse, transition_frames);
    max_position_ -= transitions_.size();

    auto encoder = vs::encoder(
            codec,
            path,
            clipping_->w(),
            clipping_->h(),
            1000,
            fps * 1000,
            bitrate);

    if (encoder->error()) {
        return false;
    }

    bool result = prog_handler_->wait([
        this,
        encoder{encoder.get()},
        from_start,
        append_reverse
    ] () -> bool {
        clipping_->player()->assync_context(
            [
                this,
                encoder,
                from_start,
                append_reverse
            ] (vs::Player *player) {
                process(player, encoder, from_start, append_reverse);
        });

        while (!clipping_->player()->context_finished()) {
            Fl::wait(0.1);
            prog_handler_->set_progress(current_position_.load(), max_position_);
            prog_handler_->set_buffer(current_buffer_.load(), clipping_->w(), clipping_->h());
        }

        return true;
    });

    buffers_.clear();
    transitions_.clear();

    return result;
}

void ClippingConversion::copy_buffer(vs::Player *player, uint8_t *buffer) {
    mempcpy(buffer, player->buffer(), player_buffer_size());
}

void ClippingConversion::encode_from_begin(vs::Player *player, vs::Encoder *encoder) {
    player->seek_frame(clipping_->first_frame());
    auto normal_interval = std::make_pair(clipping_->first_frame(), clipping_->last_frame());

    uint32_t save_pos = 0;
    uint32_t use_pos = 0;

    normal_interval.first += transitions_.size();
    normal_interval.second -= transitions_.size();

    float alpha = transparency_increment();

    ConversionBuffer render_buffer(clipping_->req_buffer_size());

    do {
        if (normal_interval.first > player->position() && !transitions_.empty() && save_pos < transitions_.size()) {
            copy_buffer(player, transitions_[save_pos]->data);
            ++save_pos;
            continue; // do not render transition frames at begin
        }

        clipping_->render(clipping_->at(player->position()), render_buffer.data);

        if (normal_interval.second < player->position() && !transitions_.empty() && use_pos < transitions_.size()) {
            clipping_->render(
                clipping_->at(player->position()),
                alpha,
                transitions_[use_pos]->data,
                render_buffer.data);
            alpha += transparency_increment();
            ++use_pos;
        }

        encode_frame(encoder, render_buffer.data);
        player->next();
    } while(player->position() < clipping_->last_frame() && !prog_handler_->canceled());
}

void ClippingConversion::encode_from_end(vs::Player *player, vs::Encoder *encoder) {

}

float ClippingConversion::transparency_increment() {
    switch(transitions_.size()) {
        case 4:
            return 0.20;
        case 3:
            return 0.25;
        case 2:
            return 0.35;
        case 1:
            return 0.50;
        default:
            return 0;
    }
}

void ClippingConversion::process(vs::Player *player, vs::Encoder *encoder, bool from_start, bool append_reverse) {
    if (from_start)
        encode_from_begin(player, encoder);
    else
        encode_from_end(player, encoder);

    if (append_reverse) {
        if (from_start)
            encode_from_end(player, encoder);
        else
            encode_from_begin(player, encoder);
    }

    encoder->finish();
}

void ClippingConversion::allocate_buffers(bool from_start, bool append_reverse, uint8_t transition_frames) {
    buffers_.clear();
    transitions_.clear();

    if (append_reverse) {
        // not necessary because reversed copies are smooth
        transition_frames = 0;
    }

    if (transition_frames > 4) {
        transition_frames = 4;
    }

    uint32_t buffer_count = 1;

    if (!from_start || append_reverse) {
        buffer_count = max_memory_ / player_buffer_size();
        if (buffer_count < 1)
            buffer_count = 1;
    }

    if (buffer_count > clipping_->duration_frames()) {
        buffer_count = clipping_->duration_frames();
    }

    if (buffer_count > transition_frames) {
        buffer_count -= transition_frames;
    }

    buffers_.reserve(buffer_count);

    for (uint32_t i = 0; i < buffer_count; ++i) {
        buffers_.push_back(
            std::shared_ptr<ConversionBuffer>(new ConversionBuffer(player_buffer_size()))
        );
    }

    transitions_.reserve(transition_frames);
    for (uint32_t i = 0; i < transition_frames; ++i) {
        transitions_.push_back(
            std::shared_ptr<ConversionBuffer>(new ConversionBuffer(player_buffer_size()))
        );
    }
}

void ClippingConversion::encode_frame(vs::Encoder *encoder, u_int8_t *buffer) {
    current_buffer_.store(buffer);
    encoder->frame(buffer);
    ++current_position_;
}

uint32_t ClippingConversion::player_buffer_size() {
    return 3 * clipping_->player()->info()->w() * clipping_->player()->info()->h();
}

}  // namespace vcutter
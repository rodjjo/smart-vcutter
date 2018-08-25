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
        clipping_->player()->async_context(
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
            prog_handler_->set_buffer(render_buffer_->data, clipping_->w(), clipping_->h());
        }

        return true;
    });

    prog_handler_->set_buffer(NULL, 0, 0);
    prog_handler_->set_progress(100, 100);
    render_buffer_.reset();
    buffers_.reset();
    transitions_.clear();

    return result;
}

void ClippingConversion::copy_buffer(vs::Player *player, uint8_t *buffer) {
    mempcpy(buffer, player->buffer(), player_buffer_size());
}

void ClippingConversion::encode_from_begin(vs::Player *player, vs::Encoder *encoder) {
    player->seek_frame(clipping_->first_frame());

    auto normal_interval = std::make_pair(clipping_->first_frame(), clipping_->last_frame());
    normal_interval.first += transitions_.size();
    normal_interval.second -= transitions_.size();

    uint32_t save_pos = 0;
    uint32_t use_pos = 0;
    float alpha = transparency_increment();

    do {
        if (normal_interval.first > player->position() && !transitions_.empty() && save_pos < transitions_.size()) {
            copy_buffer(player, transitions_[save_pos]->data);
            ++save_pos;
            continue; // do not render transition frames at the beginning
        }

        clipping_->render(clipping_->at(player->position()), render_buffer_->data);

        if (normal_interval.second <= player->position() && !transitions_.empty() && use_pos < transitions_.size()) {
            clipping_->render(
                clipping_->at(player->position()),
                alpha,
                transitions_[use_pos]->data,
                render_buffer_->data);
            alpha += transparency_increment();
            ++use_pos;
        }

        encode_frame(encoder, render_buffer_->data);
        player->next();
    } while(player->position() < clipping_->last_frame() && !prog_handler_->canceled());
}

void ClippingConversion::encode_from_end(vs::Player *player, vs::Encoder *encoder) {
    uint32_t remaining = clipping_->duration_frames() - transitions_.size();

    if (!remaining) {
        return;
    }

    auto normal_interval = std::make_pair(clipping_->first_frame(), clipping_->last_frame());
    normal_interval.first += transitions_.size();
    normal_interval.second -= transitions_.size();


    uint8_t *buffer = NULL;
    bool accumulating = true;
    uint32_t save_pos = 0;
    uint32_t use_pos = 0;
    uint32_t position = clipping_->last_frame() - buffers_->count();
    uint32_t render_pos = clipping_->last_frame();
    float alpha = transparency_increment();

    size_t accumulate_count = 0;

    if (position < clipping_->first_frame())
        position = clipping_->first_frame();
    player->seek_frame(position);

    do {
        if (accumulating) {
            if (accumulate_count < clipping_->duration_frames() && buffers_->push(player->buffer())) {
                ++accumulate_count;
                player->next();
                continue;
            }
            accumulating = false;
        }

        buffer = buffers_->pop();

        if (!buffer) {
            if (accumulate_count >= clipping_->duration_frames()) {
                break;
            }

            if (position - buffers_->count() >= clipping_->first_frame()) {
                position -= buffers_->count();
            } else {
                position = clipping_->first_frame();
            }
            accumulating = true;
            player->seek_frame(position);
            continue;
        }

        if (normal_interval.second <= render_pos && !transitions_.empty() && save_pos < transitions_.size()) {
            memcpy(transitions_[save_pos]->data, buffer, player_buffer_size());
            ++save_pos;
        } else {
            clipping_->render(clipping_->at(render_pos), buffer, render_buffer_->data);

            if (remaining <= transitions_.size() && !transitions_.empty() && use_pos < transitions_.size()) {
                clipping_->render(
                    clipping_->at(render_pos),
                    alpha,
                    transitions_[use_pos]->data,
                    render_buffer_->data);
                alpha += transparency_increment();
                ++use_pos;
            }

            encode_frame(encoder, render_buffer_->data);
        }

        --render_pos;
        --remaining;
    } while (!prog_handler_->canceled() && remaining);
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
    buffers_.reset();
    transitions_.clear();

    render_buffer_.reset(new CharBuffer(clipping_->req_buffer_size()));

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

    buffers_.reset(new FifoBuffer(player_buffer_size(), buffer_count));

    transitions_.reserve(transition_frames);
    for (uint32_t i = 0; i < transition_frames; ++i) {
        transitions_.push_back(
            std::shared_ptr<CharBuffer>(new CharBuffer(player_buffer_size()))
        );
    }
}

void ClippingConversion::encode_frame(vs::Encoder *encoder, uint8_t *buffer) {
    encoder->frame(buffer);
    ++current_position_;
}

uint32_t ClippingConversion::player_buffer_size() {
    return 3 * clipping_->player()->info()->w() * clipping_->player()->info()->h();
}

}  // namespace vcutter

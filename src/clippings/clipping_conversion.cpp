/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <Fl/Fl.H>
#include "src/clippings/clipping_iterator.h"
#include "src/clippings/clipping_conversion.h"
#include "src/common/utils.h"

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
    last_encoded_buffer_.store(NULL);
    current_alpha_ = 1.0;
    alpha_increment_ = 0;
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
    transitions_.clear();
    last_encoded_buffer_.store(NULL);

    if (append_reverse) {
        transition_frames = 0;
    } else if (transition_frames > 5) {
        transition_frames = 5;
    }

    define_transition_settings(transition_frames);

    auto encoder = vs::encoder(codec, path, clipping_->w(), clipping_->h(), 1000, fps * 1000, bitrate);

    if (encoder->error()) {
        return false;
    }

    bool result = prog_handler_->wait([this, encoder{encoder.get()}, from_start, append_reverse, transition_frames] () -> bool {
        ClippingIterator clip_iter(clipping_.get(), max_memory_);

        clip_iter.iterate(from_start, append_reverse, [this, encoder, transition_frames] (uint8_t *buffer) -> bool {

            if (transitions_.size() < transition_frames) {
                transitions_.push_back(std::shared_ptr<CharBuffer>(new CharBuffer(clipping_->req_buffer_size())));
                memcpy((*transitions_.rbegin())->data, buffer, clipping_->req_buffer_size());
                return !prog_handler_->canceled();
            } else if (transition_frames != 0 && current_position_.load() + transition_frames >= max_position_) {
                auto it = transitions_.begin();
                std::advance(it, current_position_.load() + transition_frames - max_position_);
                combine_buffers(buffer, (*it)->data);
            }

            encode_frame(encoder, buffer);

            return !prog_handler_->canceled();
        });

        while (!clip_iter.finished()) {
            prog_handler_->set_buffer(last_encoded_buffer_.load(), clipping_->w(), clipping_->h());
            prog_handler_->set_progress(current_position_.load(), max_position_);
            Fl::wait(0.1);
        }

        prog_handler_->set_buffer(NULL, 0, 0);
        prog_handler_->set_progress(100, 100);

        return true;
    });

    return result;
}

void ClippingConversion::define_transition_settings(uint32_t transition_count) {
    const float intervals[6] = {0, 0.5, 0.35, 0.25, 0.2, 0.15};
    current_alpha_ = intervals[transition_count % 6];
    alpha_increment_ = current_alpha_;
}

void ClippingConversion::combine_buffers(uint8_t *primary_buffer, uint8_t *secondary_buffer) {
    cv::Mat output(clipping_->h(), clipping_->w(), CV_8UC3, primary_buffer);
    cv::Mat source(clipping_->h(), clipping_->w(), CV_8UC3, secondary_buffer);
    cv::addWeighted(source, current_alpha_, output, 1.0 - current_alpha_, 0.0, output);
    current_alpha_ += alpha_increment_;
    if (current_alpha_ > 1.0) {
        current_alpha_ = 1;
    }
}

void ClippingConversion::encode_frame(vs::Encoder *encoder, uint8_t *buffer) {
    last_encoded_buffer_.store(buffer); // the clipping iterator does not release any buffer before it destroy itself.
    encoder->frame(buffer);
    ++current_position_;
}

}  // namespace vcutter

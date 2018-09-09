/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <Fl/Fl.H>
#include "src/clippings/clipping_conversion.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/common/utils.h"

namespace vcutter {


ClippingConversion::ClippingConversion(
    std::shared_ptr<ProgressHandler> prog_handler,
    std::shared_ptr<ClippingRender> clipping,
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


bool ClippingConversion::prepare_conversion(
    const char *codec,
    const char *path,
    uint32_t bitrate,
    double fps,
    bool append_reverse
) {
    current_position_.store(0);
    max_position_ = clipping_->duration_frames() * (append_reverse ? 2 : 1);
    transitions_.clear();
    last_encoded_buffer_.store(NULL);

    encoder_ = vs::encoder(codec, path, clipping_->w(), clipping_->h(), 1000, fps * 1000, bitrate);

    if (encoder_->error()) {
        error_ = encoder_->error();
        return false;
    } else {
        clip_iter_.reset(new ClippingIterator(clipping_.get(), max_memory_));
    }

    return true;
}

void ClippingConversion::unprepare_conversion() {
    encoder_.reset();
    clip_iter_.reset();
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
    if (append_reverse) {
        transition_frames = 0;
    }

    if (prepare_conversion(codec, path, bitrate, fps, append_reverse)) {
        start_conversion(from_start, append_reverse, transition_frames);

        return wait_conversion();
    }

    return false;
}

void ClippingConversion::start_conversion(bool from_start, bool append_reverse, uint8_t transition_frames) {
    define_transition_settings(&transition_frames);

    auto frame_handler_function = [this, transition_frames] (uint8_t *buffer) -> bool {
        if (transitions_.size() < transition_frames) {
            transitions_.push_back(std::shared_ptr<CharBuffer>(new CharBuffer(clipping_->req_buffer_size())));
            memcpy((*transitions_.rbegin())->data, buffer, clipping_->req_buffer_size());
            ++current_position_;
            return !prog_handler_->canceled();
        } else if (transition_frames != 0 && current_position_.load() + transition_frames >= max_position_) {
            auto it = transitions_.begin();
            std::advance(it, current_position_.load() + transition_frames - max_position_);
            combine_buffers(buffer, (*it)->data);
        }

        encode_frame(buffer);
        return !prog_handler_->canceled();
    };

    clip_iter_->iterate(from_start, append_reverse, frame_handler_function);
}

bool ClippingConversion::wait_conversion() {
    bool result = prog_handler_->wait([this] () -> bool {
        while (!clip_iter_->finished()) {
            prog_handler_->set_buffer(last_encoded_buffer_.load(), clipping_->w(), clipping_->h());
            prog_handler_->set_progress(current_position_.load(), max_position_);
            Fl::wait(0.1);
        }
        return true;
    });

    unprepare_conversion();

    return result;
}

void ClippingConversion::define_transition_settings(uint8_t *transition_frames) {
     if (*transition_frames > max_position_) {
        *transition_frames = max_position_ - 1;
    }

    if (*transition_frames > 5) {
        *transition_frames = 5;
    }

    const float intervals[6] = {0, 0.5, 0.35, 0.25, 0.2, 0.15};
    current_alpha_ = intervals[*transition_frames % 6];
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

void ClippingConversion::encode_frame(uint8_t *buffer) {
    last_encoded_buffer_.store(buffer); // the clipping iterator does not release any buffer before it destroy itself.
    encoder_->frame(buffer);
    ++current_position_;
}

const char * ClippingConversion::error() const {
    if (error_.empty()) {
        return NULL;
    }

    return error_.c_str();
}

}  // namespace vcutter

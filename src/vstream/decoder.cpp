/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/vstream/decoder.h"

namespace vs {

DecoderImp::DecoderImp(const char* path, source_type origin) {
    origin_ = origin;
    stream_.reset(new vs::FFMpegStream());
    if (!stream_->open(path)) {
        error_ = "Could not open ";
        error_ += path;
        stream_.reset();
    }
}

DecoderImp::~DecoderImp() {

}

source_type DecoderImp::source() {
    return origin_;
}

uint32_t DecoderImp::w() {
    if (stream_) {
        return stream_->get_width();
    }
    return 0;
}

uint32_t DecoderImp::h() {
    if (stream_) {
        return stream_->get_height();
    }
    return 0;
}

const char* DecoderImp::error() {
    if (error_.length()) {
        return error_.c_str();
    }
    return NULL;
}

unsigned char *DecoderImp::buffer() {
    if (stream_) {
        return *(stream_->get_picture());
    }
    return NULL;
}

uint32_t DecoderImp::buffer_size() {
    if (stream_) {
        return stream_->get_picture_buffer_size();
    }
    return 0;
}

uint32_t DecoderImp::position() {
    if (stream_) {
        return stream_->get_frame_number();
    }
    return 0;
}

uint32_t DecoderImp::count() {
    if (stream_) {
        return stream_->get_frame_count();
    }
    return 0;
}

double DecoderImp::fps() {
    if (stream_) {
        return stream_->get_fps();
    }
    return 0;
}

double DecoderImp::duration() {
    if (stream_) {
        return stream_->get_duration();
    }
    return 0;
}

double DecoderImp::time() {
    if (stream_) {
        return stream_->get_frame_time();
    }
    return 0;
}

int64_t DecoderImp::pts() {
    if (stream_) {
        return stream_->get_frame_pts();
    }
    return 0;
}

int DecoderImp::ratio_den() {
    if (stream_) {
        return stream_->get_aspect_ratio_denominator();
    }
    return 1;
}

int DecoderImp::ratio_num() {
    if (stream_) {
        return stream_->get_aspect_ratio_numerator();
    }

    return 1;
}

int DecoderImp::time_den() {
    if (stream_) {
        return stream_->get_time_base_denominator();
    }
    return 1;
}

int DecoderImp::time_num() {
    if (stream_) {
        return stream_->get_time_base_numerator();
    }
    return 1;
}

bool DecoderImp::key_frame() {
    if (stream_) {
        return stream_->is_key_frame();
    }
    return false;
}

void DecoderImp::next() {
    if (stream_) {
        stream_->next_frame();
    }
}

void DecoderImp::prior() {
    uint32_t frame_number = position();
    if (frame_number > 0) {
        seek_frame(frame_number - 1);
    }
}

void DecoderImp::seek_frame(int64_t frame) {
    if (stream_) {
        stream_->seek_frame(frame);
    }
}

void DecoderImp::seek_time(int64_t ms_time) {
    if (stream_) {
        stream_->seek_time(ms_time);
    }
}

}  // namespace vs

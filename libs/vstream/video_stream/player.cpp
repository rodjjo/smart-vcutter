#include "video_stream/player.h"

namespace vs {

PlayerImp::PlayerImp(const char* path, source_type origin) {
    origin_ = origin;
    stream_.reset(new vs::FFMpegStream());
    if (!stream_->open(path)) {
        error_ = "Could not open ";
        error_ += path;
        stream_.reset();
    }
}

PlayerImp::~PlayerImp() {

}

source_type PlayerImp::source() {
    return origin_;
}

uint32_t PlayerImp::w() {
    if (stream_) {
        return stream_->get_width();
    }
    return 0;
}

uint32_t PlayerImp::h() {
    if (stream_) {
        return stream_->get_height();
    }
    return 0;
}

const char* PlayerImp::error() {
    if (error_.length()) {
        return error_.c_str();
    }
    return NULL;
}

unsigned char *PlayerImp::buffer() {
    if (stream_) {
        return *(stream_->get_picture());
    }
    return NULL;
}

uint32_t PlayerImp::buffer_size() {
    if (stream_) {
        return stream_->get_picture_buffer_size();
    }
    return 0;
}

uint32_t PlayerImp::position() {
    if (stream_) {
        return stream_->get_frame_number();
    }
    return 0;
}

uint32_t PlayerImp::count() {
    if (stream_) {
        return stream_->get_frame_count();
    }
    return 0;
}

double PlayerImp::fps() {
    if (stream_) {
        return stream_->get_fps();
    }
    return 0;
}

double PlayerImp::duration() {
    if (stream_) {
        return stream_->get_duration();
    }
    return 0;
}

double PlayerImp::time() {
    if (stream_) {
        return stream_->get_frame_time();
    }
    return 0;
}

int64_t PlayerImp::pts() {
    if (stream_) {
        return stream_->get_frame_pts();
    }
    return 0; 
}

int PlayerImp::ratio_den() {
    if (stream_) {
        return stream_->get_aspect_ratio_denominator();
    }
    return 1;
}

int PlayerImp::ratio_num() {
    if (stream_) {
        return stream_->get_aspect_ratio_numerator();
    }

    return 1;
}

int PlayerImp::time_den() {
    if (stream_) {
        return stream_->get_time_base_denominator();
    }
    return 1;
}

int PlayerImp::time_num() {
    if (stream_) {
        return stream_->get_time_base_numerator();
    }
    return 1;
}

bool PlayerImp::key_frame() {
    if (stream_) {
        return stream_->is_key_frame();
    }
    return false;
}

void PlayerImp::next() {
    if (stream_) {
        stream_->next_frame();
    }
}

void PlayerImp::prior() {
    uint32_t frame_number = position();
    if (frame_number > 0) {
        seek_frame(frame_number - 1);
    }
}

void PlayerImp::seek_frame(int64_t frame) {
    if (stream_) {
        stream_->seek_frame(frame);
    }
}

void PlayerImp::seek_time(int64_t ms_time) {
    if (stream_) {
        stream_->seek_time(ms_time);
    }
}

}
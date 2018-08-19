/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <Fl/Fl.H>


#include "src/wrappers/video_player.h"

namespace vcutter {


PlayerWrapper::PlayerWrapper(const std::string& path) {
    player_ = vs::open_file(path.c_str());
    frame_changed_.store(true);
    conversion_finished_.store(true);
    finished_ = false;
    playing_ = false;
    playing_interval_ = false;
    start_ = 0;
    end_ = 0;
    set_speed(1);
    thread_.reset(new boost::thread([this] () {
        run();
    }));
}

PlayerWrapper::~PlayerWrapper() {
    finished_ = true;
    thread_->join();
}

vs::StreamInfo *PlayerWrapper::info() {
    return player_.get();
}

void PlayerWrapper::set_speed(float speed) {
    speed_.store(floor(speed * 100 + 0.5));
}

float PlayerWrapper::get_speed() {
    return speed_.load() / 100.0;
}

void PlayerWrapper::play() {
    stop_playing();
    playing_ = true;
}

bool PlayerWrapper::frame_changed(bool clear_flag) {
    if (clear_flag) {
        return frame_changed_.exchange(false) == true;
    }
    return frame_changed_.load();
}

bool PlayerWrapper::conversion_finished() {
    return conversion_finished_.load();
}

bool PlayerWrapper::is_playing() {
    return playing_ || playing_interval_;
}

bool PlayerWrapper::is_playing_interval() {
    return playing_interval_;
}

void PlayerWrapper::play(unsigned int start, unsigned int end) {
    stop_playing();
    if (start >= end) {
        return;
    }
    start_ = start;
    end_ = end;
    playing_interval_ = true;
}

void PlayerWrapper::stop_playing() {
    if (playing_ || playing_interval_) {
        playing_ = false;
        playing_interval_ = false;
        noop();
    }
}

void PlayerWrapper::noop() {
    call_async([](){});
}

void PlayerWrapper::stop() {
    stop_playing();
    call_async([this] () {
        player_->seek_frame(0);
        frame_changed_.store(true);
    });
}

void PlayerWrapper::next() {
    stop_playing();
    call_async([this] () {
        player_->next();
        frame_changed_.store(true);
    });
}

void PlayerWrapper::prior() {
    stop_playing();
    call_async([this] () {
        player_->prior();
        frame_changed_.store(true);
    });
}

void PlayerWrapper::seek_frame(int64_t frame) {
    call_async([this, frame] () {
        player_->seek_frame(frame);
        frame_changed_.store(true);
    });
}

void PlayerWrapper::seek_time(int64_t ms_time) {
    call_async([this, ms_time] () {
        player_->seek_time(ms_time);
        frame_changed_.store(true);
    });
}

void PlayerWrapper::replace_callback(async_callback_t callback) {
    while (!mtx_run_.try_lock()) {
        Fl::wait(0.1);
    }
    boost::lock_guard<boost::mutex> lock_guard(mtx_run_, boost::adopt_lock_t());
    callback_ = callback;
}

void PlayerWrapper::wait_callback() {
    while (true) {
        {
            boost::lock_guard<boost::mutex> lock_guard(mtx_run_);
            if (!callback_) {
                break;
            }
        }
        Fl::wait(0.1);
    }
}

void PlayerWrapper::call_async(async_callback_t callback) {
   replace_callback(callback);
   wait_callback();
}

void PlayerWrapper::run_callback() {
    boost::lock_guard<boost::mutex> lock_guard(mtx_run_);
    if (callback_) {
        callback_();
        callback_ = async_callback_t();
    }
}

void PlayerWrapper::change_speed(bool increment) {
    float step = 0.20 * (increment ? 1: -1);

    if (get_speed() + step <= 5.0 && get_speed() + step >= 0.20) {
        set_speed(get_speed() + step);
    } else {
        if (step > 0) {
            set_speed(0.20);
        } else {
            set_speed(5.0);
        }
    }
}

bool PlayerWrapper::grab_frame() {
    if (!playing_ && !playing_interval_) {
        return false;
    }

    if (playing_interval_) {
        if (info()->position() >= end_ ||
            info()->position() < start_
        ) {
            player_->seek_frame(start_);
        } else {
            player_->next();
        }

        frame_changed_.store(true);
    } else {
        frame_changed_.store(true);
        player_->next();

        if (info()->position() >= info()->count()) {
            playing_ = false;
        }
    }

    auto speed = get_speed();

    if (speed <= 0.20) {
        speed = 0.20;
    }

    if (speed <= 10) {
        double fps = info()->fps();

        if (fps < 1) {
            fps = 1;
        } else if (fps > 60) {
            fps = 60;
        }

        boost::this_thread::sleep(
            boost::posix_time::milliseconds(1000.0 / (fps * speed))
        );
    }
    return true;
}

void PlayerWrapper::pause() {
    stop_playing();
}

void PlayerWrapper::conversion_thread(async_conversion_t callback) {
    conversion_finished_.store(false);
    replace_callback([this, callback] () {
        callback(player_.get());
        conversion_finished_.store(true);
    });
}

void PlayerWrapper::run() {
    while (!finished_) {
        run_callback();
        if (grab_frame()) {
            continue;
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
    }
}


}  // namespace vcutter

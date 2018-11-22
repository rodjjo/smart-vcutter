/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <Fl/Fl.H>


#include "src/player/player.h"

namespace vcutter {

const float kON_FRAME_TIMEOUT_INTERVAL = 0.0333;

Player::Player(const char *path) {
    init(path);
}

Player::Player(const char *path, frame_callback_t frame_changed_cb) {
    frame_changed_cb_ = frame_changed_cb;
    init(path);
    init_frame_changed_notifier();
}

void Player::init(const char *path) {
    decoder_ = vs::open_file(path);
    frame_changed_.store(true);
    execution_finished_.store(true);
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

void Player::init_frame_changed_notifier() {
    if (frame_changed_cb_) {
        Fl::add_timeout(kON_FRAME_TIMEOUT_INTERVAL, &Player::timeout_handler, this);
    }
}

void Player::timeout_handler(void* ud) {
    if (static_cast<Player *>(ud)->frame_changed(true)) {
        static_cast<Player *>(ud)->frame_changed_cb_(static_cast<Player *>(ud));
    }
    Fl::repeat_timeout(kON_FRAME_TIMEOUT_INTERVAL, &Player::timeout_handler, ud);
}

Player::~Player() {
    if (frame_changed_cb_) {
        Fl::remove_timeout(&Player::timeout_handler, this);
    }
    finished_ = true;
    thread_->join();
}

vs::StreamInfo *Player::info() {
    return decoder_.get();
}

void Player::set_speed(float speed) {
    speed_.store(floor(speed * 100 + 0.5));
}

float Player::get_speed() {
    return speed_.load() / 100.0;
}

void Player::play() {
    stop_playing();
    playing_ = true;
}

bool Player::frame_changed(bool clear_flag) {
    if (clear_flag) {
        return frame_changed_.exchange(false) == true;
    }
    return frame_changed_.load();
}

bool Player::execution_finished() {
    return execution_finished_.load();
}

bool Player::is_playing() {
    return playing_ || playing_interval_;
}

bool Player::is_playing_interval() {
    return playing_interval_;
}

void Player::play(unsigned int start, unsigned int end) {
    stop_playing();
    if (start >= end) {
        return;
    }
    start_ = start;
    end_ = end;
    playing_interval_ = true;
}

void Player::stop_playing() {
    if (playing_ || playing_interval_) {
        playing_ = false;
        playing_interval_ = false;
        noop();
    }
}

void Player::noop() {
    call_async([](){});
}

void Player::stop() {
    stop_playing();
    call_async([this] () {
        decoder_->seek_frame(0);
        frame_changed_.store(true);
    });
}

void Player::next() {
    stop_playing();
    call_async([this] () {
        decoder_->next();
        frame_changed_.store(true);
    });
}

void Player::prior() {
    stop_playing();
    call_async([this] () {
        decoder_->prior();
        frame_changed_.store(true);
    });
}

void Player::seek_frame(int64_t frame) {
    call_async([this, frame] () {
        decoder_->seek_frame(frame);
        frame_changed_.store(true);
    });
}

void Player::seek_time(int64_t ms_time) {
    call_async([this, ms_time] () {
        decoder_->seek_time(ms_time);
        frame_changed_.store(true);
    });
}

void Player::replace_callback(async_callback_t callback) {
    while (!mtx_run_.try_lock()) {
        Fl::wait(0.1);
    }
    boost::lock_guard<boost::mutex> lock_guard(mtx_run_, boost::adopt_lock_t());
    callback_ = callback;
}

void Player::wait_callback() {
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

void Player::call_async(async_callback_t callback) {
   replace_callback(callback);
   wait_callback();
}

void Player::run_callback() {
    boost::lock_guard<boost::mutex> lock_guard(mtx_run_);
    if (callback_) {
        callback_();
        callback_ = async_callback_t();
    }
}

void Player::change_speed(bool increment) {
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

bool Player::grab_frame() {
    if (!playing_ && !playing_interval_) {
        return false;
    }

    if (playing_interval_) {
        if (info()->position() >= end_ ||
            info()->position() < start_
        ) {
            decoder_->seek_frame(start_);
        } else {
            decoder_->next();
        }

        frame_changed_.store(true);
    } else {
        frame_changed_.store(true);
        decoder_->next();

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

void Player::pause() {
    stop_playing();
}

void Player::execute(context_callback_t callback) {
    execution_finished_.store(false);
    replace_callback([this, callback] () {
        callback(decoder_.get());
        execution_finished_.store(true);
    });
}

void Player::run() {
    while (!finished_) {
        run_callback();
        if (grab_frame()) {
            continue;
        }
        boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
    }
}


}  // namespace vcutter

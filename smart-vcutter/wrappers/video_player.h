/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_WRAPPERS_VIDEO_PLAYER_H_
#define SMART_VCUTTER_WRAPPERS_VIDEO_PLAYER_H_

#include <atomic>
#include <functional>
#include <boost/thread.hpp>
#include "video_stream/vstream/video_stream.h"

namespace vcutter {

typedef std::function<void()> async_callback_t;

class PlayerWrapper {
 public:
    PlayerWrapper(const std::string& path);
    virtual ~PlayerWrapper();
    vs::StreamInfo *info();
    void play();
    void set_speed(float speed);
    float get_speed();
    void play(unsigned int start, unsigned int end);
    void pause();
    void stop();
    void next();
    void prior();
    bool change_speed(bool increment);
    void seek_frame(int64_t frame);
    void seek_time(int64_t ms_time);
    bool is_playing();
    bool is_playing_interval();
    bool frame_changed(bool clear_flag);
  private:
    void replace_callback(async_callback_t callback);
    void wait_callback();
    void call_async(async_callback_t callback);
    void run();
    void noop();
    void stop_playing();
    void run_callback();
    bool grab_frame();
  private:
    bool finished_;
    bool playing_;
    bool playing_interval_;
    std::atomic_int speed_;
    std::atomic_bool frame_changed_;
    unsigned int start_;
    unsigned int end_;
    std::shared_ptr<vs::Player> player_;
    std::shared_ptr<boost::thread> thread_;
    boost::mutex mtx_run_;
    async_callback_t callback_;
};

}  // namespace vcutter

#endif // SMART_VCUTTER_WRAPPERS_VIDEO_PLAYER_H_

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_PLAYER_PLAYER_H_
#define SRC_PLAYER_PLAYER_H_

#include <atomic>
#include <functional>
#include <boost/thread.hpp>
#include "src/vstream/video_stream.h"

namespace vcutter {

class Player;

typedef std::function<void()> async_callback_t;

typedef std::function<void(vs::Decoder *decoder)> context_callback_t;

typedef std::function<void(Player *player)> frame_callback_t;

class Player {
 public:
    Player(const char *path);
    Player(const char *path, frame_callback_t frame_changed_cb);
    virtual ~Player();
    vs::StreamInfo *info();
    void play();
    void set_speed(float speed);
    float get_speed();
    void play(unsigned int start, unsigned int end);
    void pause();
    void stop();
    void next();
    void prior();
    void change_speed(bool increment);
    void seek_frame(int64_t frame);
    void seek_time(int64_t ms_time);
    bool is_playing();
    bool is_playing_interval();
    bool execution_finished();
    void execute(context_callback_t callback);
    void set_frame_changed_callback(frame_callback_t frame_changed_cb);
    void clear_frame_changed_callback();
  private:
    void init(const char *path);
    void init_frame_changed_notifier();
    static void timeout_handler(void* ud);
    bool frame_changed(bool clear_flag);
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
    std::atomic_bool execution_finished_;
    unsigned int start_;
    unsigned int end_;
    std::shared_ptr<vs::Decoder> decoder_;
    std::shared_ptr<boost::thread> thread_;
    boost::mutex mtx_run_;
    async_callback_t callback_;
    frame_callback_t frame_changed_cb_;
};

}  // namespace vcutter

#endif // SRC_PLAYER_PLAYER_H_

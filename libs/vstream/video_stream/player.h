/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef VIDEO_STREAM_PLAYER_H_
#define VIDEO_STREAM_PLAYER_H_

#include <memory>

#include "video_stream/vstream/video_stream.h"
#include "video_stream/ffmpeg_stream.h"

namespace vs {

class PlayerImp: public vs::Player {
 public:
    PlayerImp(const char* path, source_type origin);
    virtual ~PlayerImp();
    source_type source() override;
    uint32_t w() override;
    uint32_t h() override;
    const char* error() override;
    unsigned char *buffer() override;
    uint32_t buffer_size() override;
    uint32_t position() override;
    uint32_t count() override;
    double fps() override;
    double duration() override;
    double time() override;
    int64_t pts() override;
    int ratio_den() override;
    int ratio_num() override;
    int time_den() override;
    int time_num() override;
    bool key_frame() override;
    void next() override;
    void prior() override;
    void seek_frame(int64_t frame) override;
    void seek_time(int64_t ms_time) override;
 private:
    std::unique_ptr<vs::FFMpegStream> stream_;
    source_type origin_;
    std::string error_;
};

}  // namespace vs

#endif  // VIDEO_STREAM_PLAYER_H_

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef VIDEO_STREAM_FFMPEG_STREAM_H_
#define VIDEO_STREAM_FFMPEG_STREAM_H_

#include <inttypes.h>
#include <string>
#include <vector>
#include "video_stream/ffmpeg_headers.h"
#include "video_stream/ffmpeg_guards.h"

namespace vs {

class FFMpegStream {
 public:
    FFMpegStream(int color_type);
    FFMpegStream();
    virtual ~FFMpegStream();
    bool open(const char *location);
    unsigned char **get_picture();
    unsigned int get_picture_buffer_size();
    bool is_mjpeg();
    bool next_frame(bool ignore_capture = false);
    bool init_codec();
    void seek_time(int64_t time);
    void seek_frame(int64_t frame);
    int get_width();
    int get_height();
    double get_fps();
    double get_duration();
    double get_frame_time();
    int64_t time_to_frame(int64_t time_value);
    int64_t get_frame_number();
    int64_t get_frame_count();
    int64_t get_frame_pts();
    int get_time_base_denominator();
    int get_time_base_numerator();
    int get_aspect_ratio_numerator(); 
    int get_aspect_ratio_denominator();
    double r2d(const AVRational& r);
    int get_color_type();
    int get_stream_color_format();
    bool is_key_frame();
    bool cancel();
 private:
    void init();
    int64_t get_frame_from_pts();

 protected:
    bool exit_;
    bool is_open_;
    bool have_new_frame_;
    bool is_mjpeg_;
    int video_stream_index_;
    int frame_width_;
    int frame_height_;
    int64_t frame_number_;
    int64_t frame_count_;
    int64_t frame_pts_;
    int64_t first_frame_;
    double duration_;
    double fps_;
    int color_type_;
    AVStream *video_stream_;
    AVCodec *video_codec_;
    AVCodecContextPtr codec_ctx_;
    SwsContextPtr sws_ctx_;
    AVFramePtr frame_;
    AVPicturePtr picture_;
    std::vector<uint8_t> video_extra_data_;
    FormatContextPtr format_ctx_;
};

}  // namespace vstream

#endif  // VIDEO_STREAM_FFMPEG_STREAM_H_
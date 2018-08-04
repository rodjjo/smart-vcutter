/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef VIDEO_STREAM_VSTREAM_VIDEO_STREAM_H_
#define VIDEO_STREAM_VSTREAM_VIDEO_STREAM_H_

#include <inttypes.h>
#include <memory>

namespace vs {

typedef enum {
    file_source = 1,
    http_source = 2 // not supported yet
} source_type;

typedef enum {
    video_color_source = 0,
    video_color_gray = 1,
    video_color_rgb = 2
} video_color_type;


class StreamInfo { 
  public:
    virtual ~StreamInfo();
    virtual source_type source() = 0;
    virtual uint32_t w() = 0;
    virtual uint32_t h() = 0;
    virtual const char* error() = 0;
    virtual unsigned char *buffer() = 0;
    virtual uint32_t buffer_size() = 0;
    virtual uint32_t position() = 0; 
    virtual uint32_t count() = 0;
    virtual double fps() = 0;
    virtual double duration() = 0;
    virtual double time() = 0;
    virtual int64_t pts() = 0;
    virtual int ratio_den() = 0;
    virtual int ratio_num() = 0;
    virtual int time_den() = 0;
    virtual int time_num() = 0;
    virtual bool key_frame() = 0;
};

class Player: public StreamInfo {
 public:
    virtual ~Player();
    virtual void next() = 0;
    virtual void prior() = 0;
    virtual void seek_frame(int64_t frame) = 0;
    virtual void seek_time(int64_t ms_time) = 0;
};

class Encoder {
 public:
    virtual ~Encoder();
    virtual bool frame(const unsigned char* buffer) = 0;
    virtual const char* error() = 0;
    virtual bool finish() = 0;
    static const char **format_names();
    static int default_bitrate(const char *format_name, unsigned int w, unsigned int h, double fps);
};

std::shared_ptr<Player> open_file(const char* path);

std::shared_ptr<Encoder> encoder(
    const char *codec_name, 
    const char *path, 
    unsigned int frame_width, 
    unsigned int frame_height,
    int fps_numerator,
    int fps_denominator,
    int bit_rate
);


void initialize();

}  // namespace vstream

#endif  // VIDEO_STREAM_VSTREAM_VIDEO_STREAM_H_
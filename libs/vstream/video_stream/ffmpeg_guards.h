/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef LIBS_VSTREAM_VIDEO_STREAM_FFMPEG_GUARDS_H_
#define LIBS_VSTREAM_VIDEO_STREAM_FFMPEG_GUARDS_H_

#include <memory>
#include "video_stream/ffmpeg_headers.h"

namespace vs {

typedef std::shared_ptr<AVFormatContext> FormatContextPtr;
typedef std::shared_ptr<AVFrame> AVFramePtr;
typedef std::shared_ptr<AVCodecContext> AVCodecContextPtr;
typedef std::shared_ptr<SwsContext> SwsContextPtr;
typedef std::shared_ptr<AVPicture> AVPicturePtr;

AVCodecContextPtr allocate_codec_context(AVCodec *codec);
FormatContextPtr allocate_format_context(AVFormatContext *ctx=NULL);

AVFramePtr allocate_frame(bool free_image_data=false);
AVFramePtr allocate_picture(enum AVPixelFormat pixel_format, int width, int height);
SwsContextPtr allocate_sws_ycbcr_context(int width, int height);
SwsContextPtr allocate_sws_yuvj_context(int width, int height);

SwsContextPtr allocate_sws_rgb_context(const AVFrame* source_frame);
AVPicturePtr allocate_rgb_picture(const AVFrame *source_frame);
SwsContextPtr allocate_sws_gray_context(const AVFrame* source_frame);
AVPicturePtr allocate_gray_picture(const AVFrame *source_frame);

}  // namespace vs

#endif  // LIBS_VSTREAM_VIDEO_STREAM_FFMPEG_GUARDS_H_
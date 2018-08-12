/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_VSTREAM_FFMPEG_HEADERS_H_
#define SRC_VSTREAM_FFMPEG_HEADERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libavutil/pixfmt.h>
#include <libavutil/avstring.h>
#include <libavutil/mathematics.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/dict.h>
#include <libavutil/parseutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/avassert.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavcodec/avfft.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
// #include <libavutil/channel_layout.h>
// #include <libavutil/timestamp.h>

#ifdef __cplusplus
}
#endif

#endif  // SRC_VSTREAM_FFMPEG_HEADERS_H_

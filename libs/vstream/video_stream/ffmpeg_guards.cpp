/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "video_stream/ffmpeg_guards.h"

namespace vs {

FormatContextPtr allocate_format_context(AVFormatContext *ctx) {
    return FormatContextPtr(
        ctx ? ctx : avformat_alloc_context(),
        [] (AVFormatContext *ctx) {
            avformat_close_input(&ctx);
            // avformat_free_context
        }
    );
}

void freeAvFrame(AVFrame* c) {
    av_frame_free(&c);
}

void freeDataAndAVFrame(AVFrame* c) {
    if (c->data[0]) {
        av_freep(&c->data[0]);
    }
    av_frame_free(&c);
}

AVFramePtr allocate_frame(bool free_image_data) {
    AVFrame *tmp = av_frame_alloc();
    if (tmp) {
        tmp->data[0] = NULL;
    }
    return AVFramePtr(tmp, free_image_data ? freeDataAndAVFrame : freeAvFrame);
}

AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height) {
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        return NULL;
    }

    return picture;
}

AVFramePtr allocate_picture(enum AVPixelFormat pixel_format, int width, int height) {
    return AVFramePtr(alloc_picture(pixel_format, width, height), freeAvFrame);
}

void freeCodecContext(AVCodecContext *c) {
    if (c) {
        // avcodec_free_context
        avcodec_close(c);
        av_free(c);
    }
}

AVCodecContextPtr allocate_codec_context(AVCodec *codec) {
    return AVCodecContextPtr(
        avcodec_alloc_context3(codec),
        freeCodecContext);
}

SwsContextPtr allocate_sws_rgb_context(const AVFrame* source_frame) {
    return SwsContextPtr(
        sws_getContext(
            source_frame->width,
            source_frame->height,
            static_cast<AVPixelFormat>(source_frame->format),
            source_frame->width,
            source_frame->height,
            AV_PIX_FMT_RGB24,
            SWS_FAST_BILINEAR,
            NULL,
            NULL,
            NULL),
        &sws_freeContext);
}

void free_picture(AVPicture* picture) {
  avpicture_free(picture);
  delete picture;
}

SwsContextPtr allocate_sws_ycbcr_context(int width, int height) {
    return SwsContextPtr(
        sws_getContext(
            width,
            height,
            AV_PIX_FMT_RGB24,
            width,
            height,
            AV_PIX_FMT_YUV420P,
            SWS_FAST_BILINEAR,
            NULL,
            NULL,
            NULL),
        &sws_freeContext);
}

SwsContextPtr allocate_sws_yuvj_context(int width, int height) {
    return SwsContextPtr(
        sws_getContext(
            width,
            height,
            AV_PIX_FMT_RGB24,
            width,
            height,
            AV_PIX_FMT_YUVJ422P,
            SWS_FAST_BILINEAR,
            NULL,
            NULL,
            NULL),
        &sws_freeContext);
}
AVPicturePtr allocate_rgb_picture(const AVFrame *source_frame) {
  AVPicturePtr picture(new AVPicture, free_picture);
  avpicture_alloc(
      picture.get(),
      AV_PIX_FMT_RGB24,
      source_frame->width,
        source_frame->height);
  return picture;
}

SwsContextPtr allocate_sws_gray_context(const AVFrame* source_frame) {
    return SwsContextPtr(
        sws_getContext(
            source_frame->width,
            source_frame->height,
            static_cast<AVPixelFormat>(source_frame->format),
            source_frame->width,
            source_frame->height,
            AV_PIX_FMT_RGB24,
            SWS_FAST_BILINEAR,
            NULL,
            NULL,
            NULL),
        &sws_freeContext);
}

AVPicturePtr allocate_gray_picture(const AVFrame *source_frame) {
  AVPicturePtr picture(new AVPicture, free_picture);
  avpicture_alloc(
      picture.get(),
      AV_PIX_FMT_RGB24,
      source_frame->width,
        source_frame->height);
    return picture;
}

}  // namespace vs

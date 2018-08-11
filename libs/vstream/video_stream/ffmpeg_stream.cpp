/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "video_stream/vstream/video_stream.h"
#include "video_stream/ffmpeg_stream.h"


namespace vs {

FFMpegStream::FFMpegStream(int color_type) {
    color_type_ = color_type;
    init();
}

FFMpegStream::FFMpegStream() {
    color_type_ = video_color_rgb;
    init();
}

void FFMpegStream::init() {
    video_stream_index_ = 0;
    is_open_ = false;
    is_mjpeg_ = false;
    video_stream_ = NULL;
    video_codec_ = NULL;
    frame_width_ = 0;
    frame_height_ = 0;
    duration_ = 0;
    first_frame_ = 0;
    frame_count_ = 0;
    fps_ = 0;
    frame_number_ = 0;
    frame_pts_ = 0;
    exit_ = false;
    have_new_frame_ = false;
    frame_ = allocate_frame();
}

FFMpegStream::~FFMpegStream() {
    cancel();
}

bool FFMpegStream::cancel() {
    exit_ = true;
}

double FFMpegStream::r2d(const AVRational& r) {
    return r.num == 0 || r.den == 0 ? 0.0 : r.num / static_cast<double>(r.den);
}

bool FFMpegStream::open(const char *location) {
    if (is_open_) {
        return true;
    }

    AVFormatContext *ctx = NULL;

    if(avformat_open_input(&ctx, location, NULL, NULL) != 0){
        return false;
    }

    format_ctx_ = allocate_format_context(ctx);

    if(avformat_find_stream_info(ctx, NULL) < 0){
        return false;
    }

    video_stream_index_ = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &video_codec_, 0);

    if (video_stream_index_ < 0) {
        video_stream_index_ = 0;
        return false;
    }

    video_stream_ = format_ctx_->streams[video_stream_index_];

    return init_codec();
}

bool FFMpegStream::is_mjpeg() {
    return is_mjpeg_;
}

bool FFMpegStream::init_codec() {
    av_read_play(format_ctx_.get());

    codec_ctx_ = allocate_codec_context(video_codec_);

    AVCodecParameters *codec_par = video_stream_->codecpar;

    avcodec_parameters_to_context(codec_ctx_.get(), codec_par);

    video_extra_data_.assign(video_stream_->codecpar->extradata,
                           video_stream_->codecpar->extradata +
                           video_stream_->codecpar->extradata_size);

    codec_ctx_->time_base = video_stream_->time_base;
    codec_ctx_->extradata = &video_extra_data_[0];
    codec_ctx_->extradata_size = video_extra_data_.size();

    frame_width_ = codec_ctx_->width;
    frame_height_ = codec_ctx_->height;

    if (avcodec_open2(codec_ctx_.get(), video_codec_, NULL) < 0) {
      codec_ctx_.reset();
      return false;
    }

    if(video_codec_->capabilities & AV_CODEC_CAP_TRUNCATED) {
      codec_ctx_->flags |= AV_CODEC_FLAG_TRUNCATED;
    }

    if(video_codec_->capabilities & AV_CODEC_FLAG2_CHUNKS) {
          codec_ctx_->flags |= AV_CODEC_FLAG2_CHUNKS;
    }

    duration_ =  static_cast<double>(video_stream_->duration) * r2d(video_stream_->time_base);

    fps_ = r2d(video_stream_->r_frame_rate);
    if (fps_ < 0.000025)
        fps_ = r2d(video_stream_->avg_frame_rate);

    is_open_ = true;

    if (next_frame()) {
        first_frame_ = get_frame_from_pts();
        is_mjpeg_ = video_codec_->id == AV_CODEC_ID_MJPEG;
        frame_number_ = 1;
        return true;
    }

    return false;
}

bool FFMpegStream::next_frame(bool ignore_capture) {
    if (!is_open_) {
        return false;
    }

    if (frame_number_ + 1 == frame_count_) {
        return false;
    }

    bool have_frame = false;
    int status = 0;

    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;

    auto pts = AV_NOPTS_VALUE;
    auto dts = AV_NOPTS_VALUE;
    int stream_index;
    do {
        if (exit_) {
            return false;
        }
        pts = AV_NOPTS_VALUE;
        dts = AV_NOPTS_VALUE;

        int status = av_read_frame(format_ctx_.get(), &packet);
        if (status < 0) {
            return false;
        }

        stream_index = packet.stream_index;

        if (packet.stream_index != video_stream_index_) {
            av_packet_unref(&packet);
            av_init_packet(&packet);
            packet.data = NULL;
            packet.size = 0;
            continue;
        }

        pts = packet.pts;
        dts = packet.dts;

        status = avcodec_send_packet(codec_ctx_.get(), &packet);

        av_packet_unref(&packet);
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;

        if (status == AVERROR(EAGAIN)) {
            continue;
        }

         if (status == AVERROR_EOF) {
            break;
        }

        if (status < 0) {
            return false;
        }

        while (!have_frame) {
            status = avcodec_receive_frame(codec_ctx_.get(), frame_.get());
            if (status == AVERROR(EAGAIN) || status == AVERROR_EOF) {
                break;
            } else if (status < 0) {
                return false;
            }
            have_frame = true;
        }

    } while (stream_index != video_stream_index_ || have_frame == false);

    have_new_frame_ = true;
    frame_count_ = video_stream_->nb_frames;

    if (frame_count_ == 0) {
        double duration = format_ctx_->duration / static_cast<double>(AV_TIME_BASE);
        if (duration < 0.000025) {
            duration = duration_;
        }
        frame_count_ = (int64_t)floor(duration * fps_ + 0.5);
    }

    frame_pts_ =  pts != static_cast<int64_t>(AV_NOPTS_VALUE) && pts ? pts : dts;

    // frame_number_ = get_frame_from_pts() - first_frame_;
    ++frame_number_;

    return true;
}

unsigned char **FFMpegStream::get_picture() {
    if (!codec_ctx_) {
        return NULL;
    }

    if (!picture_) {
        have_new_frame_ = true;
        if (color_type_ == video_color_rgb) {
            picture_ = allocate_rgb_picture(frame_.get());
        } else if (color_type_ == video_color_gray) {
            picture_ = allocate_gray_picture(frame_.get());
        }
    }

    if (!sws_ctx_) {
        if (color_type_ == video_color_rgb) {
            sws_ctx_ = allocate_sws_rgb_context(frame_.get());
        } else if (color_type_ == video_color_gray) {
            sws_ctx_ = allocate_sws_gray_context(frame_.get());
        }
    }

    if (have_new_frame_ && picture_) {
        have_new_frame_ = false;
        sws_scale(
            sws_ctx_.get(),
            frame_->data,
            frame_->linesize,
            0,
            frame_->height,
            picture_->data,
            picture_->linesize);
    }

    if (!picture_) {
        return frame_->data;
    }

    return picture_->data;
}

double FFMpegStream::get_frame_time() {
  return static_cast<double>(frame_pts_ - video_stream_->start_time) * r2d(video_stream_->time_base);
}

int64_t FFMpegStream::get_frame_from_pts() {
  double sec = get_frame_time();
  return (int64_t)(fps_ * sec + 0.5);
}

void FFMpegStream::seek_frame(int64_t frame) {
    int64_t frame2seek = frame;
    if (frame2seek > frame_count_) {
      frame2seek = frame_count_;
    }

    frame_number_  = 0;

    if (!frame_) {
        return;
    }

    int delta = 16;

    for(;;) {
      int64_t frame2seek_temp = std::max(frame2seek - delta, (int64_t)0);
      double sec = frame2seek_temp / static_cast<double>(fps_);
      int64_t time_stamp = video_stream_->start_time;
      double  time_base  = r2d(video_stream_->time_base);
      time_stamp += (int64_t)(sec / time_base + 0.5);

      av_seek_frame(format_ctx_.get(), video_stream_index_, time_stamp, AVSEEK_FLAG_BACKWARD);
      avcodec_flush_buffers(codec_ctx_.get());

      if( frame2seek > 0 ) {
        next_frame();

          if( frame2seek > 1 ) {
              frame_number_ = get_frame_from_pts() - first_frame_;

              if( frame_number_ < 0 || frame_number_ > frame2seek-1 )
              {
                  if (frame2seek_temp == 0 || delta >= INT_MAX/4)
                      break;
                  delta = delta < 16 ? delta*2 : delta*3/2;
                  continue;
              }
              while( frame_number_ < frame2seek - 1 ) {
                if (!next_frame())
                  break;
              }
              frame_number_++;
              break;
          }
          else
          {
              frame_number_ = 1;
              break;
          }
      }
      else
      {
          frame_number_ = 0;
          break;
      }
  }

  if (frame == 0) {
    next_frame();
  }
}

int64_t FFMpegStream::time_to_frame(int64_t time_value) {
    return (int64_t)((time_value / 1000.0f) * fps_ + 0.5);
}

void FFMpegStream::seek_time(int64_t time) {
    if (!frame_) {
        return;
    }

    if (time == 0) {
        frame_number_ = 0;
        av_seek_frame(format_ctx_.get(), video_stream_index_, 0, AVSEEK_FLAG_BACKWARD);
        // avformat_seek_file(format_ctx_.get(), video_stream_index_, 0, 0, 0, 0);
        avcodec_flush_buffers(codec_ctx_.get());
        return;
    }

    seek_frame(time_to_frame(time));
}

unsigned int FFMpegStream::get_picture_buffer_size() {
    if (frame_) {
        return av_image_get_buffer_size(
            static_cast<AVPixelFormat>(frame_->format), frame_width_, frame_height_, 1);
    }
    return 0;
}

int FFMpegStream::get_width() {
    return frame_width_;
}

int FFMpegStream::get_height() {
    return frame_height_;
}

double FFMpegStream::get_fps() {
    return fps_;
}

double FFMpegStream::get_duration() {
    return duration_;
}

int64_t FFMpegStream::get_frame_number() {
    return frame_number_;
}

int64_t FFMpegStream::get_frame_pts() {
    return frame_pts_;
}

int FFMpegStream::get_time_base_denominator() {
    if (video_stream_) {
        return video_stream_->time_base.num;
    }
    return 1;
}

int FFMpegStream::get_time_base_numerator() {
    if (video_stream_) {
        return video_stream_->time_base.den;
    }
    return 1;
}

int FFMpegStream::get_aspect_ratio_numerator() {
    if (video_stream_) {
        return video_stream_->sample_aspect_ratio.num;
    }
    return 1;
}

int FFMpegStream::get_aspect_ratio_denominator() {
    if (video_stream_) {
        return video_stream_->sample_aspect_ratio.den;
    }
    return 1;
}

int64_t FFMpegStream::get_frame_count() {
    return frame_count_;
}

int FFMpegStream::get_color_type() {
    return color_type_;
}

int FFMpegStream::get_stream_color_format() {
    if (frame_) {
        return frame_->format;
    }
    return 0;
}

bool FFMpegStream::is_key_frame() {
    if (frame_) {
        return frame_->key_frame != 0;
    }
    return false;
}

}  // namespace vs

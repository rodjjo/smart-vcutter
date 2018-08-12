/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/vstream/encoder.h"

namespace vs {

namespace {

const char *kVP9_CODEC = "webm";
const char *kX264_CODEC = "mp4-x264";
const char *kX265_CODEC = "mp4-x265";
const char *kAV1_CODEC = "aom-av1";
const char *kMJPEG_CODEC = "mjpeg";

const char *kFORMAT_NAMES[] = {
    kVP9_CODEC,
    kX265_CODEC,
    kMJPEG_CODEC,
    kX264_CODEC,
//    kAV1_CODEC,  // future release
    NULL
};

}  // namespace


const char **Encoder::format_names() {
    return kFORMAT_NAMES;
}

int Encoder::default_bitrate(const char *format_name, unsigned int w, unsigned int h, double fps) {
    uint64_t raw_size = fps * w * h;
    float ratio = 0;
    const int motion_rank = 3; // value from 1 to 4 (4 = hight camera motion, 1 few camera motion)

    if (strcmp(format_name, kMJPEG_CODEC) == 0) {
        ratio = 3.0f / 5.5f;
    } else if (strcmp(format_name, kX264_CODEC) == 0) {
        ratio = motion_rank * 0.07;
    } else if (strcmp(format_name, kX265_CODEC) == 0) {
        ratio = motion_rank * 0.04;
    } else if (strcmp(format_name, kVP9_CODEC) == 0) {
        ratio = motion_rank * 0.065;
    } else if (strcmp(format_name, kAV1_CODEC) == 0) {
        ratio = motion_rank * 0.035;
    }

    return raw_size * ratio;
}


EncoderImp::EncoderImp(
    const char *codec_name,
    const char *path,
    unsigned int frame_width,
    unsigned int frame_height,
    int fps_numerator,
    int fps_denominator,
    int bit_rate
) {
    opened_ = false;
    finished_ = false;
    should_close_file_ = false;
    stream_ = NULL;
    codec_ = NULL;
    codec_name_ = codec_name;
    path_ = path;
    frame_pts_ = 0;
    got_packet_ptr_ = 0;
    frame_width_ = frame_width;
    frame_height_ = frame_height;
    fps_numerator_ = fps_numerator;
    fps_denominator_ = fps_denominator;
    bit_rate_ = bit_rate;
    key_frame_interval_ = 10;
    max_bidirectional_frames_ = 1;
    frame_align_ = 32;
    init_encoder();
}

EncoderImp::~EncoderImp() {
    if (opened_ && !finished_) {
        finish();
    }
    if (format_ctx_ && should_close_file_) {
        avio_closep(&format_ctx_->pb);
    }
}

bool EncoderImp::frame(const unsigned char* buffer) {
    if (!opened_) {
        report_error("Encoder is not opened");
        return false;
    }

    int ret = av_frame_make_writable(frame_.get());
    if (ret < 0) {
        return false;
    }

    frame_->pts = frame_pts_++;

    AVFrame source_frame;
    memset(&source_frame, 0, sizeof(source_frame));
    source_frame.data[0] =  const_cast<unsigned char*>(buffer);
    source_frame.format = AV_PIX_FMT_RGB24;
    source_frame.width  = frame_width_;
    source_frame.height = frame_height_;
    source_frame.linesize[0] = frame_width_ * 3;

    sws_scale(
            output_color_context_.get(),
            source_frame.data,
            source_frame.linesize,
            0,
            source_frame.height,
            frame_->data,
            frame_->linesize);

    if (!encode_frame(frame_.get())) {
        return false;
    }

    if (codec_->id != AV_CODEC_ID_H264) {
        return flush_frames();
    }

    return true;
}

const char* EncoderImp::error() {
    if (error_.length()) {
        return error_.c_str();
    }
    return NULL;
}

bool EncoderImp::finish() {
    if (!opened_) {
        return false;
    }
    opened_ = false;
    finished_ = true;
    if (codec_->id == AV_CODEC_ID_H264) {
        if (!flush_frames()) {
            return false;
        }
    }

    av_write_trailer(format_ctx_.get());

    codec_ctx_.reset();

    return true;
}

bool EncoderImp::find_codec() {
    AVCodecID codec_id = AV_CODEC_ID_NONE;

    if (strcmp(kMJPEG_CODEC, codec_name_.c_str()) == 0) {
        codec_id = AV_CODEC_ID_MJPEG;
    } else if (strcmp(kX264_CODEC, codec_name_.c_str()) == 0) {
        codec_id = AV_CODEC_ID_H264;
    } else if (strcmp(kX265_CODEC, codec_name_.c_str()) == 0) {
        codec_id = AV_CODEC_ID_HEVC;
    } else if (strcmp(kVP9_CODEC, codec_name_.c_str()) == 0) {
        codec_id = AV_CODEC_ID_VP9;
    } else if (strcmp(kAV1_CODEC, codec_name_.c_str()) == 0) {
        codec_id = AV_CODEC_ID_AV1;
    } else {
        report_error("Invalid codec name");
        return NULL;
    }

    codec_ = avcodec_find_encoder(codec_id);

    if (!codec_) {
        report_error("Could not find a supported codec");
    }

    return codec_ != NULL;
}

const char *EncoderImp::find_format() {
    if (strcmp(kMJPEG_CODEC, codec_name_.c_str()) == 0) {
        return "mp4";
    } else if (strcmp(kX264_CODEC, codec_name_.c_str()) == 0) {
        return "mp4";
    } else if (strcmp(kX265_CODEC, codec_name_.c_str()) == 0) {
        return "mp4";
    } else if (strcmp(kVP9_CODEC, codec_name_.c_str()) == 0 ||
               strcmp(kAV1_CODEC, codec_name_.c_str()) == 0) {
        return "webm";
    }
    report_error("Could not find a supported output format");
    return NULL;
}


bool EncoderImp::allocate_format() {
    const char *format_name = find_format();

    if (!format_name) {
        return false;
    }

    AVFormatContext *context;
    avformat_alloc_output_context2(&context, NULL, format_name, NULL);

    if (!context) {
        report_error("Could not allocate format context");
        return false;
    }

    format_ctx_ = vs::allocate_format_context(context);
    return true;
}

bool EncoderImp::allocate_stream() {
    if (!find_codec()) {
        return false;
    }

    stream_ = avformat_new_stream(format_ctx_.get(), NULL);

    if (stream_) {
        stream_->id = format_ctx_->nb_streams-1;
        stream_->time_base.num = fps_numerator_;
        stream_->time_base.den = fps_denominator_;
    } else {
        report_error("Could not allocate the video stream");
    }

    return stream_ != NULL;
}

bool EncoderImp::configure_codec() {
    if (!allocate_format()) {
        return false;
    }

    if (!allocate_stream()) {
        return false;
    }

    codec_ctx_ = vs::allocate_codec_context(codec_);
    if (!codec_ctx_) {
        report_error("Could not allocate a context for the codec");
        return false;
    }

    codec_ctx_->codec_id = codec_->id;
    codec_ctx_->width = frame_width_;
    codec_ctx_->height = frame_height_;
    codec_ctx_->time_base.num = fps_numerator_;
    codec_ctx_->time_base.den = fps_denominator_;

    if (codec_->id == AV_CODEC_ID_MJPEG) {
        codec_ctx_->pix_fmt = AV_PIX_FMT_YUVJ422P;
    } else {
        codec_ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
        codec_ctx_->max_b_frames = max_bidirectional_frames_;
    }

    codec_ctx_->gop_size = key_frame_interval_;

    codec_ctx_->bit_rate = bit_rate_ ;
     codec_ctx_->bit_rate_tolerance = bit_rate_ * 0.05;

    if (format_ctx_->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (codec_->id == AV_CODEC_ID_H264 || codec_->id == AV_CODEC_ID_HEVC) {
        av_opt_set(codec_ctx_->priv_data, "preset", "slow", 0);
    } else if (codec_->id == AV_CODEC_ID_VP9) {
        av_opt_set(codec_ctx_->priv_data, "cpu-used", "1", AV_OPT_SEARCH_CHILDREN);
    }

    int ret;
    if ((ret = avcodec_open2(codec_ctx_.get(), codec_, NULL)) < 0) {
        report_error("Could not open codec");
        return false;
    }

    if (avcodec_parameters_from_context(stream_->codecpar, codec_ctx_.get()) < 0) {
        report_error("Could not configure media stream");
        return false;
    }

    return true;
}

void EncoderImp::init_encoder() {
    if (frame_width_ % 2 != 0) {
        report_error("The video width must be multiple of 2");
        return;
    }

    if (!configure_codec()) {
        return;
    }

    if (!this->allocate_frame()) {
        return;
    }

    if (!open_output_file()) {
        return;
    }

    opened_ = true;
}

bool EncoderImp::allocate_frame() {
    frame_ = vs::allocate_picture(codec_ctx_->pix_fmt, codec_ctx_->width, codec_ctx_->height);

    if (!frame_) {
        report_error("Could not allocate the frame image");
        return false;
    }

    if (codec_->id == AV_CODEC_ID_MJPEG) {
        output_color_context_ = vs::allocate_sws_yuvj_context(codec_ctx_->width, codec_ctx_->height);
    } else {
        output_color_context_ = vs::allocate_sws_ycbcr_context(codec_ctx_->width, codec_ctx_->height);
    }
    if (!output_color_context_) {
        report_error("Could not allocate color conversion context");
        return false;
    }

    return true;
}

bool EncoderImp::open_output_file() {
    av_dump_format(format_ctx_.get(), 0, path_.c_str(), 1);
    AVDictionary *opt = NULL;

    if (format_ctx_->oformat->flags & AVFMT_NOFILE) {
        report_error("Error occurred when opening output file");
        return false;
    } else if (avio_open(&format_ctx_->pb, path_.c_str(), AVIO_FLAG_WRITE) < 0) {
        report_error("Could not open the output file");
        return false;
    } else if (avformat_write_header(format_ctx_.get(), &opt) < 0) {
        report_error("Could write to the output file");
        return false;
    }
    should_close_file_ = true;
    return true;
}

bool EncoderImp::encode_frame(AVFrame *frame) {
    AVPacket packet = {0};
    av_init_packet(&packet);
    packet.data = NULL;    // packet data will be allocated by the encoder
    packet.size = 0;

    if (avcodec_encode_video2(codec_ctx_.get(), &packet, frame, &got_packet_ptr_) < 0) {
        report_error("Error encoding frame");
        return false;
    }

    if (!frame) {
        av_frame_free(NULL);
    }

    if (got_packet_ptr_) {
        av_packet_rescale_ts(&packet, codec_ctx_->time_base, stream_->time_base);
        packet.stream_index = stream_->index;
        if (av_interleaved_write_frame(format_ctx_.get(), &packet) < 0) {
            report_error("Error writing frame");
            return false;
        }
    }

    return true;
}

bool EncoderImp::flush_frames() {
    got_packet_ptr_ = 1;

    while (got_packet_ptr_) {
        if (!encode_frame(NULL)) {
            return false;
        }
    }

    return true;
}

void EncoderImp::report_error(const char *error) {
    error_ = error;
    opened_ = false;
}

}  // namespace vs

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WRAPPERS_VIDEO_CONVERSION_H_
#define SRC_WRAPPERS_VIDEO_CONVERSION_H_

#include <functional>
#include <string>
#include <atomic>
#include <memory>
#include "src/clippings/clipping.h"

namespace vcutter {

class VideoConversionWrapper {
 public:
    VideoConversionWrapper(
        const char *source_path,
        unsigned int start_frame,
        unsigned int end_frame,
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps);

    VideoConversionWrapper(
        std::shared_ptr<Clipping> clipping,
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps,
        bool start_at_end);

    void convert(bool append_reverse = false, bool merge_reverse = false, bool ask_exists = true);
 private:
    void init(
        const char* codec_name,
        const char *target_path,
        int bitrate,
        double fps);
    void conversion_thread(vs::Player *player);
    void allocate_buffers();
    void release_buffers();
    const unsigned char* preview_buffer();
    int preview_w();
    int preview_h();
    uint32_t interval();
    void flush_buffers(vs::Encoder *encoder, int count, bool from_start, bool discart_first);
    void encode_all(vs::Player *player, vs::Encoder *encoder);
    void encode_from_start(vs::Player *player, vs::Encoder *encoder);
    void encode_from_end(vs::Player *player, vs::Encoder *encoder);
    void keep_first_frame(vs::Player *player);
 private:
    std::vector<std::shared_ptr<unsigned char> > buffers_;
    uint32_t buffer_size_;
    std::atomic_int buffer_index_;

    std::string codec_name_;
    std::string target_path_;

    int target_w_;
    int target_h_;

    uint32_t start_frame_;
    uint32_t end_frame_;
    bool buffering_;
    bool encode_error_;
    bool canceled_;
    int bitrate_;
    double fps_;
    bool clipping_start_at_end_;
    bool append_reverse_;
    bool merge_frames_;
    ClippingKey first_key_;
    std::shared_ptr<unsigned char> first_frame_;
    std::shared_ptr<Clipping> clipping_;
    std::atomic_int position_;
    std::atomic_int count_;
};

}  // namespace vcutter

#endif  // SRC_WRAPPERS_VIDEO_CONVERSION_H__

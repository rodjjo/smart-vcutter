/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_VSTREAM_DECODER_H_
#define SRC_VSTREAM_DECODER_H_

#include <memory>

#include "src/vstream/video_stream.h"
#include "src/vstream/ffmpeg_stream.h"

namespace vs {

class DecoderImp: public vs::Decoder {
 public:
    DecoderImp(const char* path, source_type origin);
    virtual ~DecoderImp();
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

#endif  // SRC_VSTREAM_DECODER_H_

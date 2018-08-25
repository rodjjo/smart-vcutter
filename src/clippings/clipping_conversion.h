/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_CONVERSION_H_
#define SRC_CLIPPINGS_CLIPPING_CONVERSION_H_

#include <inttypes.h>
#include <functional>
#include <memory>
#include <atomic>
#include <boost/core/noncopyable.hpp>
#include "src/clippings/clipping.h"
#include "src/common/buffers.h"
#include "src/vstream/video_stream.h"

namespace vcutter {

typedef std::function<bool()> progress_task_t;

class ProgressHandler {
 public:
    virtual ~ProgressHandler(){}
    virtual bool wait(progress_task_t task) = 0;
    virtual bool canceled() = 0;
    virtual void set_buffer(uint8_t *buffer, uint32_t w, uint32_t h) = 0;
    virtual void set_progress(uint32_t progress, uint32_t max_progress) = 0;
};

class ClippingConversion: private boost::noncopyable {
 public:
    ClippingConversion(std::shared_ptr<ProgressHandler> prog_handler, std::shared_ptr<Clipping> clipping, uint32_t max_memory=419430400);

    bool convert(
        const char *codec,
        const char *path,
        uint32_t bitrate,
        double fps,
        bool from_start=true,
        bool append_reverse=false,
        uint8_t transition_frames=0);

 private:
    void allocate_buffers(bool from_start, bool append_reverse, uint8_t transition_frames);
    void process(vs::Player *player, vs::Encoder *encoder, bool from_start, bool append_reverse);
    void encode_frame(vs::Encoder *encoder, uint8_t *buffer);
    void encode_from_begin(vs::Player *player, vs::Encoder *encoder);
    void encode_from_end(vs::Player *player, vs::Encoder *encoder);
    void copy_buffer(vs::Player *player, uint8_t *buffer);
    float transparency_increment();
    uint32_t player_buffer_size();

 private:
    std::atomic<uint32_t> current_position_;
    uint32_t max_position_;
    std::unique_ptr<CharBuffer> render_buffer_;
    std::shared_ptr<Clipping> clipping_;
    std::shared_ptr<ProgressHandler> prog_handler_;
    std::unique_ptr<FifoBuffer> buffers_;
    std::vector<std::shared_ptr<CharBuffer> > transitions_;
    uint32_t max_memory_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_CONVERSION_H_

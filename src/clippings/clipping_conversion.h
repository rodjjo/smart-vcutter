/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_CONVERSION_H_
#define SRC_CLIPPINGS_CLIPPING_CONVERSION_H_

#include <inttypes.h>
#include <functional>
#include <memory>
#include <list>
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
    ClippingConversion(std::shared_ptr<ProgressHandler> prog_handler, std::shared_ptr<ClippingRender> clipping, uint32_t max_memory=419430400);

    bool convert(
        const char *codec,
        const char *path,
        uint32_t bitrate,
        double fps,
        bool from_start=true,
        bool append_reverse=false,
        uint8_t transition_frames=0);

 private:
    void encode_frame(vs::Encoder *encoder, uint8_t *buffer);
    void copy_buffer(vs::Player *player, uint8_t *buffer);
    float transparency_increment();
    void combine_buffers(uint8_t *primary_buffer, uint8_t *secondary_buffer);
    void define_transition_settings(uint32_t transition_count);
 private:
    std::atomic<uint32_t> current_position_;
    std::atomic<uint8_t*> last_encoded_buffer_;
    uint32_t max_position_;
    std::shared_ptr<ClippingRender> clipping_;
    std::shared_ptr<ProgressHandler> prog_handler_;
    std::list<std::shared_ptr<CharBuffer> > transitions_;
    float current_alpha_;
    float alpha_increment_;
    uint32_t max_memory_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_CONVERSION_H_

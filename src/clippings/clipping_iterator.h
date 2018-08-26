/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_ITERATOR_H_
#define SRC_CLIPPINGS_CLIPPING_ITERATOR_H_

#include <inttypes.h>
#include <atomic>
#include <functional>
#include <list>
#include "src/clippings/clipping.h"
#include "src/common/buffers.h"

namespace vcutter {

typedef std::function<bool(uint8_t *output_buffer)> frame_iteration_cb_t;

class ClippingIterator {
 public:
    ClippingIterator(Clipping *clipping, uint32_t max_memory);
    virtual ~ClippingIterator() {}
    void iterate(bool from_start, bool append_reverse, frame_iteration_cb_t cb);
    bool finished();
 private:
    uint32_t buffer_count(uint32_t frame_count);
    void grab_all(vs::Player *player, uint32_t from_frame, uint32_t to_frame, bool append_reverse, frame_iteration_cb_t cb);
    void from_begin(vs::Player *player, bool append_reverse, uint32_t from_frame, uint32_t to_frame, frame_iteration_cb_t cb);
    void from_end(vs::Player *player, bool append_reverse, uint32_t from_frame, uint32_t to_frame, frame_iteration_cb_t cb);
    void render_frame(uint8_t *buffer);
    void report_frames(bool forward, bool append_reverse, frame_iteration_cb_t cb);
    bool flush_buffers(frame_iteration_cb_t cb);

 private:
    Clipping *clipping_;
    std::unique_ptr<FifoBuffer> buffers_;
    std::unique_ptr<CharBuffer> render_buffer_;
    std::list<std::shared_ptr<CharBuffer> > frames_;
    uint32_t max_memory_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_ITERATOR_H_

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include "src/clippings/clipping_iterator.h"

namespace vcutter {

ClippingIterator::ClippingIterator(ClippingRender *clipping, uint32_t max_memory) {
    max_memory_ = max_memory;
    clipping_ = clipping;
    render_buffer_.reset(new CharBuffer(clipping_->req_buffer_size()));
}


uint32_t ClippingIterator::buffer_count(uint32_t frame_count) {
    uint32_t result =  max_memory_ / clipping_->req_buffer_size();

    if (result > frame_count) {
        return frame_count;
    }

    if (result < 0) {
        return 1;
    }

    return result;
}

void ClippingIterator::iterate(bool from_start, bool append_reverse, frame_iteration_cb_t cb) {
    uint32_t from_frame = from_start ? clipping_->first_frame() : clipping_->last_frame();
    uint32_t to_frame = from_start ? clipping_->last_frame() : clipping_->first_frame();

    clipping_->player()->async_context([
        this,
        from_frame,
        to_frame,
        append_reverse,
        cb
    ] (vs::Player *player) {
        uint32_t frame_count = (from_frame > to_frame) ? from_frame - to_frame : to_frame - from_frame;

        if (frame_count < 1) {
            return;
        }

        if (from_frame > to_frame || append_reverse) {
            uint32_t count = buffer_count(frame_count);

            if (count == frame_count) {
                grab_all(player, from_frame, to_frame, append_reverse, cb);
                return;
            }

            buffers_.reset(new FifoBuffer(clipping_->req_buffer_size(), count));
        }

        render_buffer_.reset(new CharBuffer(clipping_->req_buffer_size()));

        if (from_frame > to_frame) {
            from_end(player, append_reverse, to_frame, from_frame, cb);
        } else {
            from_begin(player, append_reverse, from_frame, to_frame, cb);
        }
    });
}

void ClippingIterator::from_begin(vs::Player *player, bool append_reverse, uint32_t from_frame, uint32_t to_frame, frame_iteration_cb_t cb) {
    uint32_t frame_count = (to_frame - from_frame) + 1;

    player->seek_frame(from_frame);
    while (frame_count) {
        render_frame(render_buffer_->data);
        if (!cb(render_buffer_->data)) {
            return;
        }
        player->next();
        --frame_count;
    }

    if (append_reverse && from_frame + 2 <= to_frame) {
        from_end(player, false, from_frame + 1, to_frame - 1, cb);
    }
}

void ClippingIterator::from_end(vs::Player *player, bool append_reverse, uint32_t from_frame, uint32_t to_frame, frame_iteration_cb_t cb) {
    uint32_t frame_count = (to_frame - from_frame) + 1;
    uint32_t position = to_frame;
    bool flushing = false;

    do {
        flushing = false;

        position -= buffers_->count();
        if (position > to_frame) {
            position = from_frame;
        } else if (position < from_frame) {
            position = from_frame;
        }

        player->seek_frame(position);

        while (!flushing) {
            --frame_count;
            render_frame(render_buffer_->data);
            player->next();
            if (buffers_->push(render_buffer_->data)) {
                continue;
            }
            flushing = true;
        }

        if (!flush_buffers(cb)) {
            return;
        }
    } while (frame_count);

    if (!flush_buffers(cb)) {
        return;
    }

    if (append_reverse && from_frame + 1<= to_frame) {
        from_begin(player, false, from_frame + 1, to_frame - 1, cb);
    }
}

bool ClippingIterator::flush_buffers(frame_iteration_cb_t cb) {
    uint8_t *buffer;

    do {
        buffer = buffers_->pop();
        if (buffer) {
            if (!cb(buffer)) {
                return false;
            }
        }
    } while (buffer);

    return true;
}

void ClippingIterator::grab_all(vs::Player *player, uint32_t from_frame, uint32_t to_frame, bool append_reverse, frame_iteration_cb_t cb) {
    bool forward = from_frame <= to_frame;
    frames_.clear();

    if (!forward) {
        uint32_t tmp = from_frame;
        from_frame = to_frame;
        to_frame = tmp;
    }

    player->seek_frame(from_frame);
    uint32_t frame_count = (to_frame - from_frame) + 1;

    do {
        frames_.push_back(std::shared_ptr<CharBuffer>(new CharBuffer(clipping_->req_buffer_size())));
        render_frame((*frames_.rbegin())->data);
        player->next();
    } while (frames_.size() < frame_count);

    report_frames(forward, append_reverse, cb);
}

void ClippingIterator::report_frames(bool forward, bool append_reverse, frame_iteration_cb_t cb) {
    if (forward) {
        for (auto it = frames_.begin(); it != frames_.end(); ++it) {
            if (append_reverse && (it == frames_.begin() || (*it)->data == (*frames_.rbegin())->data)) {
                continue;
            }
            if (!cb((*it)->data)) {
                return;
            }
        }
    } else {
        for (auto it = frames_.rbegin(); it != frames_.rend(); ++it) {
            if (append_reverse && (it == frames_.rbegin() || (*it)->data == (*frames_.begin())->data)) {
                continue;
            }
            if (!cb((*it)->data)) {
                return;
            }
        }
    }

    if (append_reverse) {
        report_frames(!forward, false, cb);
    }
}

void ClippingIterator::render_frame(uint8_t *buffer) {
    clipping_->render(clipping_->at(clipping_->player()->info()->position()), buffer);
}

bool ClippingIterator::finished() {
    return clipping_->player()->context_finished();
}

}  // namespace vcutter

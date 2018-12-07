/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include "src/common/buffers.h"

namespace vcutter {

StackBuffer::StackBuffer(uint32_t individual_size, uint32_t buffer_count) {
    buffer_index_ = 0;
    individual_size_ = individual_size;
    buffers_.reserve(buffer_count);

    for (uint32_t i = 0; i < buffer_count; ++i) {
        buffers_.push_back(std::shared_ptr<CharBuffer>(new CharBuffer(individual_size)));
    }
}

bool StackBuffer::push(uint8_t *buffer) {
    if (buffer_index_ < buffers_.size()) {
        memcpy(buffers_[buffer_index_]->data, buffer, individual_size_);
        ++buffer_index_;
        return buffer_index_ < buffers_.size();
    }
    return false;
}

uint8_t *StackBuffer::pop() {
    if (buffer_index_) {
        --buffer_index_;
        return buffers_[buffer_index_]->data;
    }
    return NULL;
}

uint32_t StackBuffer::count() {
    return buffers_.size();
}

}  // namespace vcutter

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_COMMON_BUFFERS_H_
#define SRC_COMMON_BUFFERS_H_

#include <memory>
#include <vector>


namespace vcutter {

class CharBuffer {
   CharBuffer(const CharBuffer&) = delete;
   CharBuffer& operator=(const CharBuffer&) = delete;
 public:
    CharBuffer(uint32_t size) {
        data = new uint8_t[size];
    }

    ~CharBuffer() {
        delete[] data;
    }

    uint8_t *data;
};


class StackBuffer {
    StackBuffer(const StackBuffer&) = delete;
    StackBuffer& operator=(const StackBuffer&) = delete;
 public:
    StackBuffer(uint32_t individual_size, uint32_t buffer_count);
    virtual ~StackBuffer(){}
    bool push(uint8_t *buffer);
    uint8_t * pop();
    uint32_t count();
 private:
    std::vector<std::shared_ptr<CharBuffer> > buffers_;
    uint32_t buffer_index_;
    uint32_t individual_size_;
};


}  // namespace vcutter

#endif // SRC_COMMON_BUFFERS_H_

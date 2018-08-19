/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_CONVERSION_H_
#define SRC_CLIPPINGS_CLIPPING_CONVERSION_H_

#include <inttypes.h>
#include <functional>
#include <memory>
#include <boost/core/noncopyable.hpp>
#include "src/clippings/clipping.h"

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

class ConversionBuffer: private boost::noncopyable  {
  public:
    ConversionBuffer(uint32_t size) {
        data = new uint8_t[size];
    }

    ~ConversionBuffer() {
        delete[] data;
    }

    uint8_t *data;
};

class ClippingConversion {
 public:
    ClippingConversion(std::shared_ptr<ProgressHandler> prog_handler, std::shared_ptr<Clipping> clipping, uint32_t max_memory=419430400);

    void convert(
        const char *codec,
        const char *path,
        uint32_t bitrate,
        double fps,
        bool from_start=true,
        bool append_reverse=false,
        uint8_t transition_frames=0);
 private:
    void allocate_buffers();

 private:
    std::shared_ptr<Clipping> clipping_;
    std::shared_ptr<ProgressHandler> prog_handler_;
    std::vector<std::shared_ptr<ConversionBuffer> > buffers_;
    uint32_t max_memory_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_CONVERSION_H_

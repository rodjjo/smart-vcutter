/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/clippings/clipping_conversion.h"

namespace vcutter {


ClippingConversion::ClippingConversion(
    std::shared_ptr<ProgressHandler> prog_handler,
    std::shared_ptr<Clipping> clipping,
    uint32_t max_memory
) {
    max_memory_ = max_memory;
    clipping_ = clipping;
}

void ClippingConversion::convert(
    const char *codec,
    const char *path,
    uint32_t bitrate,
    double fps,
    bool from_start,
    bool append_reverse,
    uint8_t transition_frames
) {
    allocate_buffers();

    buffers_.clear();
}

void ClippingConversion::allocate_buffers() {
    buffers_.clear();
}



}  // namespace vcutter

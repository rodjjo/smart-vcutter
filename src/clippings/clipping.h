
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_H_
#define SRC_CLIPPINGS_CLIPPING_H_

#include "src/clippings/clipping_render.h"

namespace vcutter {

class Clipping: public ClippingRender {
 public:
    Clipping(const Json::Value * root, frame_callback_t frame_cb);
    Clipping(const char *path, bool path_is_video, frame_callback_t frame_cb);
    virtual ~Clipping(){}
    ClippingRef & ref();
 private:
    ClippingRef clipping_ref_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_H_

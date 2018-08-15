
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_H_
#define SRC_CLIPPINGS_CLIPPING_H_

#include "src/clippings/clipping_render.h"

namespace vcutter {

class Clipping: public ClippingRender {
 public:
    explicit Clipping(const Json::Value * root);
    Clipping(const char *path, bool path_is_video);
    virtual ~Clipping(){}
    ClippingRef & ref();
 private:
    ClippingRef clipping_ref_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_H_

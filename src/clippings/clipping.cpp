/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/clippings/clipping.h"

namespace vcutter {


Clipping::Clipping(const char *path, bool path_is_video, frame_callback_t frame_cb) : ClippingRender(path, path_is_video, frame_cb), clipping_ref_(this){
}

Clipping::Clipping(const Json::Value * root, frame_callback_t frame_cb) : ClippingRender(root, frame_cb), clipping_ref_(this) {
}

ClippingRef & Clipping::ref() {
    return clipping_ref_;
}

}  // namespace vcutter

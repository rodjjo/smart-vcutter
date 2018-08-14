
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_H_
#define SRC_CLIPPINGS_CLIPPING_H_

#include <inttypes.h>
#include <string>
#include <list>

#include <jsoncpp/json/json.h>

#include "src/wrappers/video_player.h"
#include "src/clippings/clipping_render.h"

namespace vcutter {

class Clipping: public ClippingRender {
 public:
    explicit Clipping(const Json::Value * root);
    Clipping(const char *path, bool path_is_video);
    ClippingRef & ref();
 private:
    ClippingRef clipping_ref_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_H_

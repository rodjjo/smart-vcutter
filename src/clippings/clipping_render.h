
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_RENDER_H_
#define SRC_CLIPPINGS_CLIPPING_RENDER_H_

#include <inttypes.h>
#include <string>
#include <list>

#include <jsoncpp/json/json.h>

#include "src/wrappers/video_player.h"
#include "src/clippings/clipping_frame.h"

namespace vcutter {

class ClippingRender: public ClippingFrame {
 public:
    explicit ClippingRender(const Json::Value * root);
    ClippingRender(const char *path, bool path_is_video);
    void render(ClippingKey key, uint32_t target_w, uint32_t target_h, uint8_t *buffer);
    void render(ClippingKey key, uint8_t *buffer);
    void render(ClippingKey key, uint8_t *player_buffer, uint8_t *buffer);
    void render_transparent(ClippingKey key, uint8_t *player_buffer, uint8_t *buffer);
 private:
    void render(ClippingKey key, uint8_t *source_buffer, uint32_t target_w, uint32_t target_h, uint8_t *buffer, bool transparent);
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_RENDER_H_


/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_FRAME_H_
#define SRC_CLIPPINGS_CLIPPING_FRAME_H_

#include <inttypes.h>

#include "src/wrappers/video_player.h"
#include "src/clippings/clipping_data.h"
#include "src/clippings/clipping_ref.h"

namespace vcutter {

class ClippingFrame: public ClippingData {
 public:
    explicit ClippingFrame(const Json::Value * root);
    ClippingFrame(const char *path, bool path_is_video);
    PlayerWrapper *player();
    bool good();
    ClippingKey current_key();
    void remove_before(uint32_t frame);
    uint32_t last_frame() override;

    void positionate_left(uint32_t frame);
    void positionate_right(uint32_t frame);
    void positionate_top(uint32_t frame);
    void positionate_bottom(uint32_t frame);
    void positionate_vertical(uint32_t frame);
    void positionate_horizontal(uint32_t frame);
    void normalize_scale(uint32_t frame);
    void align_left(uint32_t frame);
    void align_right(uint32_t frame);
    void align_top(uint32_t frame);
    void align_bottom(uint32_t frame);
    void align_all(uint32_t frame);
 protected:
    uint32_t default_w() override;
    uint32_t default_h() override;

 private:
    void video_open();

 private:
    std::unique_ptr<PlayerWrapper> player_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_FRAME_H_

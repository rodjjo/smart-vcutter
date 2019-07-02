
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_FRAME_H_
#define SRC_CLIPPINGS_CLIPPING_FRAME_H_

#include <inttypes.h>

#include "src/player/player.h"
#include "src/clippings/clipping_data.h"
#include "src/clippings/clipping_ref.h"

namespace vcutter {

class ClippingFrame: public ClippingData {
 public:
    ClippingFrame(const Json::Value * root, frame_callback_t frame_cb);
    ClippingFrame(const char *path, bool path_is_video, frame_callback_t frame_cb);
    virtual ~ClippingFrame(){}
    Player *player();
    bool good();
    ClippingKey current_key();
    void positionate_left(uint32_t frame);
    void positionate_right(uint32_t frame);
    void positionate_top(uint32_t frame);
    void positionate_bottom(uint32_t frame);
    void center_vertical(uint32_t frame);
    void center_horizontal(uint32_t frame);
    void normalize_scale(uint32_t frame);
    void fit_left(uint32_t frame);
    void fit_right(uint32_t frame);
    void fit_top(uint32_t frame);
    void fit_bottom(uint32_t frame);
    void fit_all(uint32_t frame);
    void fit_vertical(uint32_t frame);
    void fit_horizontal(uint32_t frame);
 protected:
    uint32_t default_w() override;
    uint32_t default_h() override;
    uint32_t frame_count() override;
    frame_callback_t frame_callback();
 private:
    void video_open();

 private:
    frame_callback_t frame_cb_;
    std::unique_ptr<Player> player_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_FRAME_H_

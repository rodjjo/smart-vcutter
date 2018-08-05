#ifndef SMART_VCUTTER_VIEWER_VIEWER_TEXTURE_H
#define SMART_VCUTTER_VIEWER_VIEWER_TEXTURE_H

#include <inttypes.h>
#include <memory>

#include "smart-vcutter/common/calcs.h"

namespace vcutter {

class ViewerTexture {
 public:
    ViewerTexture();
    ViewerTexture(const uint8_t *buffer, int w, int h, bool resize_texture=false, bool rgba=false);
    virtual ~ViewerTexture();
    void update(const uint8_t *buffer, int w, int h, bool resize_texture=false, bool rgba=false);
    void draw(const viewport_t &vp, float x, float y, float zoom);
    void draw(const viewport_t &vp, const uint8_t *buffer=NULL, int w=0, int h=0, bool resize_texture=false, bool rgba=false);
    void draw(const viewport_t &vp, int vw, int vh, box_t texture_coords, box_t view_coords, float alpha);
 private:
    void update_texture(const viewport_t &vp, const uint8_t* buffer, int w, int h, bool resize_texture, bool rgba);

 private:
    uint32_t texture_id_;
    uint32_t texture_w_;
    uint32_t texture_h_;
    bool resize_texture_;
    std::shared_ptr<uint8_t> buffer_;
    bool rgba_;
    int buffer_w_;
    int buffer_h_;
    int view_w_;
    int view_h_;
};

}

#endif  // SMART_VCUTTER_VIEWER_VIEWER_TEXTURE_H
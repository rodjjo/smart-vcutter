#ifndef SMART_VCUTTER_COMMON_VIEW_PORT_H
#define SMART_VCUTTER_COMMON_VIEW_PORT_H

#include <inttypes.h>
#include "smart-vcutter/common/point.h"
#include "smart-vcutter/common/box.h"


namespace vcutter {

class viewport_t {
 public:
    viewport_t();
    viewport_t(const int *vp);
    viewport_t(int x, int y, int w, int h);

    // do not call outside opengl context
    static viewport_t from_gl(); 
    // refresh view port
    void update();

    int & operator[] (int index) {
        return vp_[index];
    }

    const int & operator[] (int index) const {
        return vp_[index];
    }

    // Convert px and py from screen coordinates to frame coordinates
    point_t screen_to_frame_coords(uint32_t video_w, uint32_t video_h, const point_t & screen_coords) const;
    box_t screen_to_frame_coords(uint32_t video_w, uint32_t video_h, const box_t & screen_coords) const;

    // Convert px and py from frame coordinates to screen coordinates
    point_t frame_to_screen_coords(uint32_t video_w, uint32_t video_h, const point_t & screen_coords) const;
    box_t frame_to_screen_coords(uint32_t video_w, uint32_t video_h, const box_t & screen_coords) const;

    // compute the pixel zoom to fit the frame in this view port
    float  raster_zoom(uint32_t video_w, uint32_t video_h) const;

    // compute the coordinate to centralize the frame in this view port (glDrawPixels and glPixelZoom)
    point_t raster_coords(uint32_t video_w, uint32_t video_h) const;

    // makes w and h smaller in order to not violate this view port limits return the scale used to fit
    float fit(uint32_t *w, uint32_t *h) const;
 private:
  int vp_[4];
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_COMMON_VIEW_PORT_H
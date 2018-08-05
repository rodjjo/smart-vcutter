#ifndef SMART_VCUTTER_COMMON_CALCS_H
#define SMART_VCUTTER_COMMON_CALCS_H

#include <inttypes.h>

#include "smart-vcutter/common/point.h"
#include "smart-vcutter/common/box.h"

namespace vcutter {


// Ensure the angle is interval 0..360
float normalize_angle(float angle);
void buffer_coords(const int *vp, uint32_t video_w, uint32_t video_h, float *px, float *py);
void screen_coords(const int *vp, uint32_t video_w, uint32_t video_h, float *px, float *py);
void raster_coordinates(const int *vp, uint32_t video_w, uint32_t video_h, float *pixel_zoom, float *raster_px, float *raster_py);

// makes w and h smaller in order to not violate view port (vp) limits (if supplied, computed_fit_scale will have the scale used to fit)
void fit_width_and_height(const int *vp, uint32_t *w, uint32_t *h, float *computed_fit_scale=NULL);


}  // namespace vcutter

#endif  // SMART_VCUTTER_COMMON_UTILS_H

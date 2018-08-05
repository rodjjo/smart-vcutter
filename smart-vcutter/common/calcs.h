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
void rescale(const int *vp, uint32_t *w, uint32_t *h, float *oscale=NULL);


}  // namespace vcutter

#endif  // SMART_VCUTTER_COMMON_UTILS_H

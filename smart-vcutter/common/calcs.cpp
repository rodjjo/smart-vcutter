#include <cmath>
#include <string.h>
#include "smart-vcutter/common/calcs.h"

#define PI 3.1415926535897
#define DEGS (PI / 180.0)

namespace vcutter {

float normalize_angle(float angle) {
    while (angle < 0) angle += 360;
    while (angle > 360) angle -= 360;
    return angle;
}

void buffer_coords(const int *vp, uint32_t video_w, uint32_t video_h, float *px, float *py) {
    float pixel_zoom, raster_px, raster_py;
    raster_coordinates(vp, video_w, video_h, &pixel_zoom, &raster_px, &raster_py);
    if (pixel_zoom) {
        float sx = 2.0f / vp[2];
        float sy = 2.0f / vp[3];
        *px -= raster_px / sx;
        *py -= raster_py / sy;
        *px /= pixel_zoom;
        *py /= pixel_zoom;
        return;
    }
    *px = -1;
    *py = -1;
}

void screen_coords(const int *vp, uint32_t video_w, uint32_t video_h, float *px, float *py) {
    float pixel_zoom, raster_px, raster_py;
    raster_coordinates(vp, video_w, video_h, &pixel_zoom, &raster_px, &raster_py);
    if (pixel_zoom) {
        float sx = 2.0f / vp[2];
        float sy = 2.0f / vp[3];
        *px *= pixel_zoom;
        *py *= pixel_zoom;
        *px += raster_px / sx;
        *py += raster_py / sy;
        return;
    }
    *px = -1;
    *py = -1;
}

void raster_coordinates(const int *vp, uint32_t video_w, uint32_t video_h, float *pixel_zoom, float *raster_px, float *raster_py) {
    double scale;
    double scale_h;
    double scaledwidth;

    if (vp[2] < 1 || vp[3] < 1) {
        *pixel_zoom = 0.0;
        *raster_px = 0;
        *raster_py = 0;
        return;
    }

    scale = vp[2] / (double)(video_w);
    scale_h = vp[3] / (double)(video_h);
    if (scale_h < scale)
        scale = scale_h;

    scaledwidth = video_w * scale;
    if (scaledwidth > vp[2])
        scale = scale * (1.0 - (scaledwidth / vp[2]));

    double scaledheight = video_h * scale;
    if (scaledheight > vp[3])
        scale = scale * (1.0 - (scaledheight / vp[3]));

    *pixel_zoom = scale;
    *raster_px = (1.0f / vp[2])  * (vp[2] - (video_w * scale));
    *raster_py = (1.0f / vp[3])  * (vp[3] - (video_h * scale));
}

void rescale(const int *vp, uint32_t *w, uint32_t *h, float *oscale) {
    if (*w <= vp[2] && *h <= vp[3]) {
        return;  // no upscale
    }

    float fx = static_cast<float>(vp[2]);
    float fy = static_cast<float>(vp[3]);
    float xscale = fx / *w;
    float scale = fy / *h;

    if (xscale < scale) {
        scale = xscale;
    }
    
    *w *= scale;
    *h *= scale;
    if (*w > fx) {
        scale *= (fx / *w);
        *w *= scale;
        *h *= scale;
    }

    if (*h > fy) {
        scale *= fy / *h;
        *w *= scale;
        *h *= scale;
    }

    if (oscale) {
        *oscale *=  (1.0 / scale);
    }
}

}  // namespace vcutter

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

void rotate_point(float angle, float *x, float *y) {
    if (!angle) {
        return;
    }

    float temp = 0;
    float s = sin(angle * DEGS);
    float c = cos(angle * DEGS);

    temp = *x;
    *x = (*x) * c - (*y) * s;
    *y = temp * s + (*y) * c;
}

void rotate_box(float angle, box_t *box) {
    for (int i = 0; i < 4; ++i) {
        rotate_point(angle, &box->p[i].x, &box->p[i].y);
    }
}

void translate_box(float px, float py, box_t *box) {
    for (int i = 0; i < 4; ++i) {
        box->p[i].x += px;
        box->p[i].y += py;
    }
}

void scale_box(float scale, box_t *box) {
    if (scale == 1.0f) {
        return;
    }
    for (int i = 0; i < 4; ++i) {
        box->p[i].x *= scale;
        box->p[i].y *= scale;
    }
}

point_t get_box_center(const box_t& box) {
    point_t r;
    r.x = 0;
    r.y = 0;
    for (int i = 0; i < 4; ++i) {
        r.x += box.p[i].x;
        r.y += box.p[i].y;
    }
    r.x /= 4;
    r.y /= 4;
    return r;
}

float getScale(float xpass, float ypass, float width, float height) {
    float scale2;
    float result = 0;
    if (((xpass > 0) || (ypass > 0)) && (width > 0) && (height > 0)) {
          result = ypass / width;
          scale2 = xpass / height;
          if (scale2 > result) {
               result = scale2;
          }
    }
    return result;
}

void compute_xypass(float width, float height, const box_t& area, point_t *lt,  point_t *rb) {
    memset(lt, 0, sizeof(point_t));
    memset(rb, 0, sizeof(point_t));

    if (area.p[0].x < 0) {
        lt->x = static_cast<int>(-area.p[0].x);
    }

    if (area.p[0].y < 0) {
        lt->y = static_cast<int>(-area.p[0].y);
    }

    if (area.p[1].x > width) {
        rb->x = static_cast<int>(area.p[1].x - width);
    }

    if (area.p[2].y > height) {
        rb->y = static_cast<int>(area.p[2].y - height);
    }
}

point_t get_box_size(const box_t& area) {
    point_t p;
    p.x = static_cast<int>(area.p[1].x - area.p[0].x);
    p.y = static_cast<int>(area.p[2].y - area.p[0].y);
    return p;
}

float get_dista(float x1, float y1, float x2, float y2) {
    float result = 0;
    float dx = x1 - x2;
    float dy = y1 - y2;
    dx = dx * dx;
    dy = dy * dy + dx;
    if (dy > 0) {
        return sqrt(dy);
    }
    return 99999999;
}

float vector_angle(double x, double y) {
    return (atan2(y, x) * (180.0 / PI)) - 90;
};

float get_angle(float x, float y) {
    return normalize_angle(vector_angle(x, y));
}

point_t to_axis(float px, float py, float x, float y) {
    point_t result;
    result.x = x - px;
    result.y = y - py;
    return result;
}

box_t get_bound_box(const box_t& box) {
    box_t result = box;
    for (int i = 0; i < 4; ++i) {
        if (box.p[i].x > result.p[1].x) {
            result.p[1].x = box.p[i].x;
        }
        if (box.p[i].x < result.p[0].x) {
            result.p[0].x = box.p[i].x;
        }
        if (box.p[i].y > result.p[2].y) {
            result.p[2].y = box.p[i].y;
        }
        if (box.p[i].y < result.p[0].y) {
            result.p[0].y = box.p[i].y;
        }
    }

    result.p[1].y = result.p[0].y;
    result.p[3].x = result.p[0].x;
    result.p[3].y = result.p[2].y;
    result.p[2].x = result.p[1].x;

    return result;
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

float sign(point_t p1, point_t p2, point_t p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool mouse_in_box(const box_t & b, int mouse_x, int mouse_y) {
    point_t pt;
    pt.x = mouse_x;
    pt.y = mouse_y;

    bool b1, b2, b3;

    b1 = sign(pt, b.p[0], b.p[1]) < 0.0f;
    b2 = sign(pt, b.p[1], b.p[2]) < 0.0f;
    b3 = sign(pt, b.p[2], b.p[0]) < 0.0f;

    if (b1 == b2 && b2 == b3) {
        return true;
    }

    b1 = sign(pt, b.p[0], b.p[2]) < 0.0f;
    b2 = sign(pt, b.p[2], b.p[3]) < 0.0f;
    b3 = sign(pt, b.p[3], b.p[0]) < 0.0f;

    return (b1 == b2 && b2 == b3);
}

}  // namespace vcutter

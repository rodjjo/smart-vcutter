#ifndef SMART_VCUTTER_COMMON_CALCS_H
#define SMART_VCUTTER_COMMON_CALCS_H

#include <inttypes.h>

namespace vcutter {

typedef struct {
    float x;
    float y;
} point_t;

typedef struct {
    point_t p[4];
} box_t;

// Ensure the angle is interval 0..360
float normalize_angle(float angle);

// rotate x and y coordinates
void rotate_point(float angle, float *x, float *y);

void rotate_box(float angle, box_t *box);
void translate_box(float px, float py, box_t *box);
void scale_box(float scale, box_t *box);
point_t get_box_center(const box_t& box);
void compute_xypass(float width, float height, const box_t& area, point_t *lt,  point_t *rb);
point_t get_box_size(const box_t& area);
float get_dista(float x1, float y1, float x2, float y2);
float get_angle(float x, float y);  // allways returns positive 
float vector_angle(double x, double y); // may return negative
point_t to_axis(float px, float py, float x, float y);
box_t get_bound_box(const box_t& box);
void buffer_coords(const int *vp, uint32_t video_w, uint32_t video_h, float *px, float *py);
void screen_coords(const int *vp, uint32_t video_w, uint32_t video_h, float *px, float *py);
void raster_coordinates(const int *vp, uint32_t video_w, uint32_t video_h, float *pixel_zoom, float *raster_px, float *raster_py);
void rescale(const int *vp, uint32_t *w, uint32_t *h, float *oscale=NULL);
bool mouse_in_box(const box_t & b, int mouse_x, int mouse_y);
float sign(point_t p1, point_t p2, point_t p3);

}  // namespace vcutter

#endif  // SMART_VCUTTER_COMMON_UTILS_H

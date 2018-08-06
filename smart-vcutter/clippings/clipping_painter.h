/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_CLIPPING_PAINTER_H
#define SMART_VCUTTER_CLIPPING_PAINTER_H

#include "smart-vcutter/data/project.h"
#include "smart-vcutter/common/calcs.h"

namespace vcutter {


clipping_key_t compute_interpolation(
    unsigned int current_frame,
    clipping_key_list_t::const_iterator begin,
    clipping_key_list_t::const_iterator end,
    bool *computed = NULL
);

void paint_clipping(
    unsigned char *souce_buffer,
    unsigned int source_w,
    unsigned int source_h,
    const clipping_key_t& interpolated_clipping,
    unsigned int target_w,
    unsigned int target_h,
    unsigned char *target_buffer,
    bool transparent=false
);

clipping_key_t adjust_bounds(
    const clipping_key_t& interpolated_clipping, unsigned int target_w, unsigned int target_h, unsigned int max_x,  unsigned int max_y);

box_t clipping_box(const clipping_key_t& interpolated_clipping, unsigned int target_w, unsigned int target_h);
box_t get_bound_box(const box_t& box);
void rotate_box(float angle, box_t *box);
void translate_box(float px, float py, box_t *box);

clipping_key_t magic_tool(
    clipping_key_t source,
    unsigned int target_w,
    unsigned int target_h,
    unsigned int max_w,
    unsigned int max_h,
    float source_rx1,
    float source_ry1,
    float source_rx2,
    float source_ry2,
    float curr_rx1,
    float curr_ry1,
    float curr_rx2,
    float curr_ry2,
    bool should_rotate,
    bool should_scale,
    bool should_positionate_x,
    bool should_positionate_y
);

}  // namespace vcutter

#endif  // SMART_VCUTTER_CLIPPING_PAINTER_H

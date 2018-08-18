/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_KEEPER_H_
#define SRC_CLIPPINGS_CLIPPING_KEEPER_H_

#include <inttypes.h>
#include <string>
#include "src/data/project.h"
#include "src/clippings/clipping_painter.h"

namespace vcutter {

// TODO(Rodrigo): REMOVE IN VERSION 1.1

class ClippingKeeper {
 public:
    ClippingKeeper();
    ClippingKeeper(const clipping_t & clip, uint32_t video_w, uint32_t video_h);
    ~ClippingKeeper();
    void load(const clipping_t & clip);
    void add_key(clipping_key_t key);
    void remove_key(int frame);
    void cutoff(int frame, bool from_begin);
    void cutoff_center(int frame);
    uint32_t get_first_frame();
    uint32_t get_last_frame();
    clipping_key_t get_key(int frame, bool *computed=NULL, box_t *bound_box=NULL);
    const clipping_key_t & get_key_at_index(uint32_t index);
    uint32_t get_key_count();
    void clear();
    clipping_key_list_t::const_iterator keys_begin();
    clipping_key_list_t::const_iterator keys_end();
    clipping_key_list_t get_keys();
    int key_index_at_frame(int frame);

    bool empty();
    std::string get_video_path();
    void set_video_path(const std::string& name, int frame_count);
    void set_dimensions(uint32_t w, uint32_t h);
    clipping_t to_clipping();
    void paint(int frame, unsigned char *souce_buffer,  unsigned char *target_buffer);
    uint32_t get_width();
    uint32_t get_height();
    uint32_t get_video_width();
    uint32_t get_video_height();
    void set_video_dimensions(uint32_t w, uint32_t h);

    void clear_modified();
    bool modified();
    uint64_t modified_version();
    bool has_ref();
    bool get_reference_frame(int *frame);
    bool get_reference(int *frame, float *rx1, float *ry1, float *rx2, float *ry2);
    void set_reference(int frame, float rx1, float ry1, float rx2, float ry2);
    void clear_reference();

    void positionate_left(int frame);
    void positionate_right(int frame);
    void positionate_top(int frame);
    void positionate_bottom(int frame);
    void center_vertical(int frame);
    void center_horizontal(int frame);
    void normalize_scale(int frame);
    void align_left(int frame);
    void align_right(int frame);
    void align_top(int frame);
    void align_bottom(int frame);
    void align_all(int frame);

 private:
    void init(const clipping_t& clipping);
    void modify_version();
 private:
    bool has_ref_;
    int ref_frame_;
    double rx1_;
    double ry1_;
    double rx2_;
    double ry2_;
    uint64_t version_;
    clipping_key_list_t keys_;
    int frame_count_;
    uint32_t video_w_;
    uint32_t video_h_;
    uint32_t w_;
    uint32_t h_;
    std::string video_path_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_KEEPER_H_

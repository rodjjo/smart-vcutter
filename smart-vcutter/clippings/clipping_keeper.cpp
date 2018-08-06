/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include <boost/filesystem.hpp>
#include "smart-vcutter/clippings/clipping_keeper.h"

namespace vcutter {

ClippingKeeper::ClippingKeeper() {
    video_w_  = 0;
    video_h_  = 0;
    w_ = 0;
    h_ = 0;
    frame_count_ = 0;
    has_ref_ = false;
    ref_frame_ = 1;
    rx1_ = 0;
    ry1_ = 0;
    rx2_ = 0;
    ry2_ = 0;
    clear_modified();
}

ClippingKeeper::ClippingKeeper(const clipping_t & clip, uint32_t video_w, uint32_t video_h) {
    video_w_ = video_w;
    video_h_ = video_h;
    clear_modified();
    init(clip);
}

ClippingKeeper::~ClippingKeeper() {
}


void ClippingKeeper::clear_modified() {
    version_ = 0;
}

clipping_key_list_t ClippingKeeper::get_keys() {
    return keys_;
}

void ClippingKeeper::init(const clipping_t& clipping) {
    w_ = clipping.w;
    h_ = clipping.h;
    video_path_ = clipping.video_path;

    has_ref_ = clipping.has_ref;
    ref_frame_ = clipping.ref_frame;
    rx1_ = clipping.rx1;
    ry1_ = clipping.ry1;
    rx2_ = clipping.rx2;
    ry2_ = clipping.ry2;

    // add items to key the frame sorted
    keys_.clear();
    for (const auto & key : clipping.items) {
        add_key(key);
    }
    clear_modified();
 }

void ClippingKeeper::load(const clipping_t& clipping) {
    init(clipping);
}

clipping_t ClippingKeeper::to_clipping() {
    clipping_t clipping;
    clipping.w = w_;
    clipping.h = h_;
    clipping.video_path = video_path_;
    clipping.items = keys_;
    clipping.has_ref = has_ref_;
    clipping.ref_frame = ref_frame_;
    clipping.rx1 = rx1_;
    clipping.ry1 = ry1_;
    clipping.rx2 = rx2_;
    clipping.ry2 = ry2_;
    return clipping;
}

bool ClippingKeeper::get_reference(int *frame, float *rx1, float *ry1, float *rx2, float *ry2) {
    if (!has_ref_) {
        return false;
    }
    for (auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->frame == ref_frame_) {
            *frame = ref_frame_;
            *rx1 = rx1_;
            *ry1 = ry1_;
            *rx2 = rx2_;
            *ry2 = ry2_;
            return true;
        }
    }
    has_ref_ = false;
    return false;
}

bool ClippingKeeper::get_reference_frame(int *frame) {
    if (!has_ref_) {
        return false;
    }
    for (auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->frame == ref_frame_) {
            *frame = ref_frame_;
            return true;
        }
    }
    has_ref_ = false;
    return false;
}

bool ClippingKeeper::has_ref() {
    if (has_ref_) {
        for (auto it = keys_.begin(); it != keys_.end(); ++it) {
            if (it->frame == ref_frame_) {
                return true;
            }
        }
        has_ref_ = false;
    }
    return false;
}

void ClippingKeeper::set_reference(int frame, float rx1, float ry1, float rx2, float ry2) {
    rx1_ = rx1;
    ry1_ = ry1;
    rx2_ = rx2;
    ry2_ = ry2;
    ref_frame_ = frame;
    has_ref_ = true;
}

void ClippingKeeper::clear_reference() {
    has_ref_ = false;
}

void ClippingKeeper::add_key(clipping_key_t key) {
    double last_angle = key.angle;
    modify_version();
    auto it = keys_.begin();
    while (it != keys_.end()) {
        if (it->frame == key.frame) {
            *it = key;
            last_angle = key.angle;
            ++it;
            return;
        } else if (it->frame > key.frame) {
            keys_.insert(it, key);
            return;
        }
        last_angle = it->angle;
        ++it;
    }

    keys_.push_back(key);
}

void ClippingKeeper::remove_key(int frame) {
    if (keys_.size() < 1)  {
        return;
    }
    modify_version();
    double last_angle = keys_.begin()->angle;
    for(auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->frame == frame) {
            keys_.erase(it);
            return;
        } else {
            last_angle = last_angle = it->angle;
        }
    }
}

clipping_key_t ClippingKeeper::get_key(int frame, bool *computed, box_t *bound_box) {
    clipping_key_t result;

    if (keys_.empty()) {
        if (computed) {
            *computed = true;
        }
        result.scale = 1.0;
        result.angle = 0;
        result.frame = frame;
        result.px = video_w_ / 2;
        result.py = video_h_ / 2;
    } else {
        result = compute_interpolation(
            frame,  keys_.cbegin(), keys_.cend(), computed);
    }

    if (bound_box) {
        auto adjusted_key = adjust_bounds(result, w_, h_, video_w_, video_h_);
        *bound_box = clipping_box(adjusted_key, w_, h_).occupied_area();
    }

    return result;
}

bool ClippingKeeper::empty() {
    return keys_.empty();
}

std::string ClippingKeeper::get_video_path() {
    return video_path_;
}

void ClippingKeeper::set_video_path(const std::string& path, int frame_count) {
    frame_count_ = frame_count;

    if (path == video_path_) {
        return;
    }

    modify_version();
    if (!path.empty()) {
        video_path_ = path;
    }
}

void ClippingKeeper::set_dimensions(uint32_t w, uint32_t h) {
    w_ = w;
    h_ = h;
    modify_version();
}

uint32_t ClippingKeeper::get_width() {
    return w_;
}

uint32_t ClippingKeeper::get_height() {
    return h_;
}

uint32_t ClippingKeeper::get_video_width() {
    return video_w_;
}

uint32_t ClippingKeeper::get_video_height() {
    return video_h_;
}

void ClippingKeeper::set_video_dimensions(uint32_t w, uint32_t h) {
    if (video_w_ == w && video_h_ == h) {
        return;
    }

    video_w_ = w;
    video_h_ = h;
    modify_version();
}

uint64_t ClippingKeeper::modified_version() {
    return version_;
}

void ClippingKeeper::modify_version() {
    ++version_;
    if (!version_)
        version_ = 1;
}


const clipping_key_t & ClippingKeeper::get_key_at_index(uint32_t index) {
    auto iterator = keys_.begin();
    std::advance(iterator, index);
    return *iterator;
}

uint32_t ClippingKeeper::get_key_count() {
    return keys_.size();
}

clipping_key_list_t::const_iterator ClippingKeeper::keys_begin() {
    return keys_.begin();
}

clipping_key_list_t::const_iterator ClippingKeeper::keys_end() {
    return keys_.end();
}

int ClippingKeeper::key_index_at_frame(int frame) {
    int index = 0;
    for (const auto & c : keys_) {
        if (c.frame == frame) {
            return index;
        }
        ++index;
    }
    return -1;
}

void ClippingKeeper::cutoff(int frame, bool from_begin) {
    modify_version();

    auto key_at_frame1 = get_key(frame);

    auto it = keys_.begin();
    while (it != keys_.end()) {
        if ((it->frame <= frame && from_begin)
            || (it->frame >= frame && !from_begin) || (!keys_.empty() &&
             (it->frame == keys_.rbegin()->frame && frame >= it->frame && from_begin)
            || (it->frame == keys_.begin()->frame && frame <= it->frame && !from_begin))
            ) {
            it = keys_.erase(it);
        } else {
            ++it;
        }
    }

    if (from_begin && frame + 1 >= frame_count_) {
        frame = frame_count_ -2;
    } else if (!from_begin && frame - 1 < 0) {
        frame = 1;
    }

    key_at_frame1.frame = frame;

    add_key(key_at_frame1);

    if (keys_.size() < 2) {
        --frame;
        if (from_begin)
            frame += 2;

        auto key_at_frame2 = get_key(frame);
        add_key(key_at_frame2);
    }
}

void ClippingKeeper::cutoff_center(int frame) {
    modify_version();
    auto key = get_key(frame);
    keys_.clear();
    add_key(key);
}

void ClippingKeeper::clear() {
    keys_.clear();
}

uint32_t ClippingKeeper::get_first_frame() {
    if (keys_.empty()) {
        return 1;
    }
    return keys_.begin()->frame;
}

uint32_t ClippingKeeper::get_last_frame() {
    if (keys_.empty()) {
        return frame_count_;
    }

    return keys_.rbegin()->frame;
}

bool ClippingKeeper::modified() {
    return version_ != 0;
}


void ClippingKeeper::positionate_left(int frame) {
    box_t bb;
    auto key = get_key(frame, NULL, &bb);
    if (bb[0].x <= 0) {
        return;
    }

    key.px -= bb[0].x;
    add_key(key);
}

void ClippingKeeper::positionate_right(int frame) {
    box_t bb;
    auto key = get_key(frame, NULL, &bb);
    if (bb[1].x >= get_video_width()) {
        return;
    }

    key.px += get_video_width() - bb[1].x;
    add_key(key);
}

void ClippingKeeper::positionate_top(int frame) {
    box_t bb;
    auto key = get_key(frame, NULL, &bb);
    if (bb[0].y <= 0) {
        return;
    }

    key.py -= bb[0].y;
    add_key(key);
}

void ClippingKeeper::positionate_bottom(int frame) {
    box_t bb;
    auto key = get_key(frame, NULL, &bb);
    if (bb[2].y >= get_video_height()) {
        return;
    }

    key.py += get_video_height() - bb[2].y;
    add_key(key);
}

void ClippingKeeper::positionate_vertical(int frame) {
    auto key = get_key(frame);
    key.py = get_video_height() / 2.0;
    add_key(key);
}

void ClippingKeeper::positionate_horizontal(int frame) {
    auto key = get_key(frame);

    key.px = get_video_width() / 2.0;
    add_key(key);
}

void ClippingKeeper::normalize_scale(int frame) {
    auto key = adjust_bounds(
            get_key(frame),
            get_width(),
            get_height(),
            get_video_width(),
            get_video_height());
    add_key(key);
}

void ClippingKeeper::align_left(int frame) {
    normalize_scale(frame);
}

void ClippingKeeper::align_right(int frame) {
    normalize_scale(frame);
}

void ClippingKeeper::align_top(int frame) {
    normalize_scale(frame);
}

void ClippingKeeper::align_bottom(int frame) {
    normalize_scale(frame);
}

void ClippingKeeper::align_all(int frame) {
    normalize_scale(frame);
}

void ClippingKeeper::paint(int frame, unsigned char *souce_buffer,  unsigned char *target_buffer) {
    paint_clipping(souce_buffer,  video_w_, video_h_, get_key(frame), w_, h_, target_buffer);
}

}  // namespace vcutter

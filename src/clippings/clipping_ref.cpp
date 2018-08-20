/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/clippings/clipping_ref.h"
#include "src/clippings/clipping_frame.h"


namespace vcutter {

ClippingRef::ClippingRef(ClippingFrame *clipping) {
    clipping_ = clipping;
    clear_reference();
}

bool ClippingRef::get_reference(uint32_t *frame, float *rx1, float *ry1, float *rx2, float *ry2) {
    if (!has_ref_) {
        return false;
    }

    for (auto it = clipping_->keys().begin(); it != clipping_->keys().end(); ++it) {
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

bool ClippingRef::get_reference_frame(uint32_t *frame) {
    if (!has_ref_) {
        return false;
    }
    for (auto it = clipping_->keys().cbegin(); it != clipping_->keys().cend(); ++it) {
        if (it->frame == ref_frame_) {
            *frame = ref_frame_;
            return true;
        }
    }
    has_ref_ = false;
    return false;
}

bool ClippingRef::has_ref() {
    if (has_ref_) {
        for (auto it = clipping_->keys().begin(); it != clipping_->keys().end(); ++it) {
            if (it->frame == ref_frame_) {
                return true;
            }
        }
        has_ref_ = false;
    }
    return false;
}

void ClippingRef::set_reference(uint32_t frame, float rx1, float ry1, float rx2, float ry2) {
    rx1_ = rx1;
    ry1_ = ry1;
    rx2_ = rx2;
    ry2_ = ry2;
    ref_frame_ = frame;
    has_ref_ = true;
}

void ClippingRef::clear_reference() {
    has_ref_ = false;
    rx1_ = 0;
    ry1_ = 0;
    rx2_ = 0;
    ry2_ = 0;
    ref_frame_ = 0;
}

}  // namespace vcutter

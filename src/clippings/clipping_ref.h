
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_REF_H_
#define SRC_CLIPPINGS_CLIPPING_REF_H_

#include <inttypes.h>
#include <boost/core/noncopyable.hpp>

namespace vcutter {

class ClippingFrame;

class ClippingRef: private boost::noncopyable {
 public:
    ClippingRef(ClippingFrame *clipping);
    bool has_ref();
    bool get_reference_frame(uint32_t *frame);
    bool get_reference(uint32_t *frame, float *rx1, float *ry1, float *rx2, float *ry2);
    void set_reference(uint32_t frame, float rx1, float ry1, float rx2, float ry2);
    void clear_reference();

 private:
    bool has_ref_;
    uint32_t ref_frame_;
    double rx1_;
    double ry1_;
    double rx2_;
    double ry2_;
    ClippingFrame *clipping_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_REF_H_

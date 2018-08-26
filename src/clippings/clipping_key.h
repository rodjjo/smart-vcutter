/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CLIPPINGS_CLIPPING_KEY_H_
#define SRC_CLIPPINGS_CLIPPING_KEY_H_

#include <jsoncpp/json/json.h>
#include "src/common/box.h"

namespace vcutter {

class ClippingFrame;
class ClippingData;

class ClippingKey {
 public:
    explicit ClippingKey(const Json::Value & data);
    ClippingKey();
    Json::Value serialize() const;
    double angle() const;
    void angle(double value);
    ClippingKey constrained(ClippingFrame *owner);
    box_t clipping_box(ClippingFrame *owner);
    bool computed();
    ClippingKey magic_tool(
        ClippingFrame *owner,
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
        bool should_positionate_y) const;
    std::string description(const ClippingKey *previous = NULL) const;

private:
    void limit_scale(ClippingFrame *owner);

private:
    friend class ClippingData;
    bool computed_;

public:
    unsigned int frame;
    uint32_t px;
    uint32_t py;
    float scale;

private:
    uint32_t angle_;
};

}  // namespace vcutter

#endif  // SRC_CLIPPINGS_CLIPPING_KEY_H_

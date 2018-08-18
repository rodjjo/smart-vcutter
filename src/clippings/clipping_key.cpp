/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cmath>
#include "src/clippings/clipping_key.h"
#include "src/clippings/clipping_frame.h"

namespace vcutter {


ClippingKey::ClippingKey(const Json::Value & data) {
    frame = data["frame"].asInt();
    px = data["px"].asInt();
    py = data["py"].asInt();
    scale = data["scale"].asInt();
    angle_ = data["angle"].asInt();
    computed_ = true;
}

ClippingKey::ClippingKey() {
    frame = 0;
    px = 0;
    py = 0;
    scale = 1;
    angle_ = 0;
    computed_ = true;
}

bool ClippingKey::computed() {
    return computed_;
}

Json::Value ClippingKey::serialize() const {
    Json::Value data;

    data["frame"] = frame;
    data["px"] = px;
    data["py"] = py;
    data["scale"] = scale;
    data["angle"] = angle_;

    return data;
}

double ClippingKey::angle() const {
    return angle_ / 1000.0;
}

void ClippingKey::angle(double value) {
    while(value > 360) value -= 360;
    while(value < 0) value += 360;
    angle_ = floor(value * 1000.0 + 0.5);
}

ClippingKey ClippingKey::constrained(ClippingFrame *owner) {
    ClippingKey result(*this);

    uint32_t max_x = owner->player()->info()->w() - 1;
    uint32_t max_y = owner->player()->info()->h() - 1;

    if (result.px < 1) {
        result.px = 1;
    }

    if (result.py < 1) {
        result.py = 1;
    }

    if (result.px + 1 >= max_x) {
        result.px = max_x - 2;
    }

    if (result.py + 1 >= max_y) {
        result.py = max_y - 2;
    }

    result.limit_scale(owner);

    return result;
}

box_t ClippingKey::clipping_box(ClippingFrame *owner) {
    box_t result;
    result[0].x = 0;
    result[0].y = 0;
    result[1].x = owner->w();
    result[1].y = 0;
    result[2].x = owner->w();
    result[2].y = owner->h();
    result[3].x = 0;
    result[3].y = owner->h();
    float half_x = owner->w() / 2.0;
    float half_y = owner->h() / 2.0;

    result.translate(-half_x, -half_y);
    result.rotate(angle());
    result.scale(scale);
    result.translate(px, py);
    result.trunc_precision();

    return result;
}

void ClippingKey::limit_scale(ClippingFrame *owner) {
    int width = owner->player()->info()->w() - 1;
    int height = owner->player()->info()->h() - 1;

    box_t area = clipping_box(owner).occupied_area();

    point_t center(area.center());

    point_t lt = area.left_top_violation(width, height);
    point_t rb = area.right_bottom_violation(width, height);

    int xpass = lt.x > rb.x ? lt.x : rb.x;
    int ypass = lt.y > rb.y ? lt.y : rb.y;

    if (xpass == 0 && ypass == 0) {
       return;
    }

    int distax = (lt.x > rb.x ? center.x + lt.x : width - center.x + rb.x);
    int distay = (lt.y > rb.y ? center.y + lt.y : height - center.y + rb.y);

    float scalex = 1.0;
    float scaley = 1.0;

    if (distax) {
        scalex = 1.0 - xpass / static_cast<float>(distax);
    }

    if (distay) {
        scaley = 1.0 - ypass / static_cast<float>(distay);
    }

    scale *= (scalex < scaley ? scalex : scaley);
}

ClippingKey ClippingKey::magic_tool(
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
        bool should_positionate_y) const {
    ClippingKey result = *this;
    ClippingKey source = *this;

    if (!(should_rotate || should_scale || should_positionate_x || should_positionate_y)) {
        return result;
    }

    double scale = 1.0;
    if (should_scale) {
        double source_distance = point_t(source_rx1, source_ry1).distance_to(source_rx2, source_ry2);
        double target_distance = point_t(curr_rx1, curr_ry1).distance_to(curr_rx2, curr_ry2);

        if (source_distance != 0) {
            scale = target_distance / source_distance;
        }
        source.scale *= scale;
    }

    if (should_positionate_x) {
        float dx = (curr_rx1 - source_rx1) * scale;
        source.px += dx;
    }

    if (should_positionate_y) {
        float dy = (curr_ry1 - source_ry1) * scale;
        source.py += dy;
    }

    if (should_rotate) {
        float source_angle = point_t(source_rx2 - source_rx1, source_ry2  - source_ry1).angle_0_360();
        float target_angle = point_t(curr_rx2 - curr_rx1, curr_ry2 - curr_ry1).angle_0_360();
        float diff = target_angle - source_angle;
        if (diff) {
            source.angle(source.angle() + diff);
        }
    }

    source = source.constrained(owner);
    box_t bb = source.clipping_box(owner).occupied_area();
    if (bb[1].x - bb[0].x > 15 && bb[2].y - bb[0].y > 15) {
        result.scale = source.scale;
    }

    if (source.px >= 1 && source.py >= 1 && source.px + 1 < owner->player()->info()->w() && source.py + 1 < owner->player()->info()->h()) {
        result.px = source.px;
        result.py = source.py;
    }

    result.angle(source.angle());

    return result;
}


}  // namespace vcutter

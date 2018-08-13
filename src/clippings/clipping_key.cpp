/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cmath>
#include "src/clippings/clipping_key.h"
#include "src/clippings/clipping.h"

namespace vcutter {

ClippingKey::ClippingKey(const Json::Value & data) {
    frame = data["frame"].asInt();
    px = data["px"].asInt();
    py = data["py"].asInt();
    scale = data["scale"].asInt();
    angle_ = data["angle"].asInt();
}

ClippingKey::ClippingKey() {
    frame = 0;
    px = 0;
    py = 0;
    scale = 1;
    angle_ = 0;
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

ClippingKey ClippingKey::constrained(Clipping *owner) {
    ClippingKey result = *this;

    int max_x = owner->player()->info()->w() - 1;
    int max_y = owner->player()->info()->h() - 1;

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

box_t ClippingKey::occupied_area(Clipping *owner) {
    return clipping_box(owner).occupied_area();
}

box_t ClippingKey::clipping_box(Clipping *owner) {
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

void ClippingKey::limit_scale(Clipping *owner) {
    int width = owner->player()->info()->w() - 1;
    int height = owner->player()->info()->h() - 1;

    box_t area = occupied_area(owner);

    point_t center(area.center());

    point_t lt = area.left_top_violation(width, height);
    point_t rb = area.right_bottom_violation(width, height);

    point_t sz = area.size();

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


}  // namespace vcutter

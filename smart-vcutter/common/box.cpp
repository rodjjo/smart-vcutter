#include "smart-vcutter/common/box.h"

namespace vcutter {

box_t box_t::rotated(float angle) const {
    box_t box(*this);
    box.rotate(angle);
    return box;
}

void box_t::rotate(float angle) {
    for (char c = 0; c < 4; ++c) p_[c].rotate(angle);
}

void box_t::translate(float px, float py) {
    for (char i = 0; i < 4; ++i) {
        (*this)[i].x += px;
        (*this)[i].y += py;
    }
}

void box_t::scale(float scale) {
    if (scale != 1.0f) {
        for (char i = 0; i < 4; ++i) {
            p_[i].x *= scale;
            p_[i].y *= scale;
        }
    }
}

point_t box_t::center() {
    point_t r;
    for (char i = 0; i < 4; ++i) {
        r.x += p_[i].x;
        r.y += p_[i].y;
    }
    r.x /= 4;
    r.y /= 4;
    return r;
}

point_t box_t::left_top_violation(float width, float height) {
    point_t coordinate;

    if (p_[0].x < 0)
        coordinate.x = static_cast<int>(-p_[0].x);

    if (p_[0].y < 0) 
        coordinate.y = static_cast<int>(-p_[0].y);

    return coordinate;
}

point_t box_t::right_bottom_violation(float width, float height) {
    point_t coordinate;

    if (p_[1].x > width) 
        coordinate.x = static_cast<int>(p_[1].x - width);

    if (p_[2].y > height) 
        coordinate.y = static_cast<int>(p_[2].y - height);

    return coordinate;
}

point_t box_t::size() {
    return point_t(p_[1].x - p_[0].x, p_[2].y - p_[0].y);
}

box_t box_t::occupied_area() {
    box_t result(*this);
    for (char i = 0; i < 4; ++i) {
        if (p_[i].x > result[1].x) 
            result[1].x = p_[i].x;
        if (p_[i].x < result[0].x) 
            result[0].x = p_[i].x;
        if (p_[i].y > result[2].y) 
            result[2].y = p_[i].y;
        if (p_[i].y < result[0].y) 
            result[0].y = p_[i].y;
    }

    result[1].y = result[0].y;
    result[3].x = result[0].x;
    result[3].y = result[2].y;
    result[2].x = result[1].x;

    return result;
}

void box_t::trunc_precision() {
    for (char i = 0; i < 4; ++i) {
        p_[i].trunc_precision();
    }
}

}  // namespace vcutter

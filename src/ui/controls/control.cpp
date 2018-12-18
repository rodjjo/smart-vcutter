/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/ui/controls/control.h"

namespace vcutter {

Control::Control() {
    rx_ = 0;
    ry_ = 0;
    rw_ = 0;
    rh_ = 0;
    window_w_ = 0;
    window_h_ = 0;
}

Control::~Control() {
}

int Control::rx() {
    return rx_;
}

int Control::ry() {
    return ry_;
}

int Control::rw() {
    return rw_;
}

int Control::rh() {
    return rh_;
}

void Control::save_reference(int window_w, int window_h) {
    rx_ = x();
    ry_ = y();
    rw_ = w();
    rh_ = h();
    window_w_ = window_w;
    window_h_ = window_h;
}

void Control::auto_size(int w, int h) {
    float sx = window_w_ / static_cast<float>(w);
    float sy = window_h_ / static_cast<float>(h);
    coordinates(rx_ * sx, ry_ * sy, rw_ * sx, rh_ * sy);
}

}  // namespace vcutter

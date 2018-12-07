/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/clippings/clipping_frame.h"

namespace vcutter {

ClippingFrame::ClippingFrame(const char *path, bool path_is_video) : ClippingData(path_is_video ? "" : path) {
    if (path_is_video) {
        video_path(path);
        video_open();
    } else {
        video_open();
    }
}

ClippingFrame::ClippingFrame(const Json::Value * root) : ClippingData(root) {
    video_open();
}

void ClippingFrame::video_open() {
    if (video_path().empty()) {
        return;
    }

    player_.reset(new Player(video_path().c_str()));

    if (!good()){
        return;
    }

    if (!w()) {
        w(player_->info()->w());
    }

    if (!h()) {
        h(player_->info()->h());
    }
}

uint32_t ClippingFrame::default_w() {
    return player_->info()->w();
}

uint32_t ClippingFrame::default_h() {
    return player_->info()->h();
}

bool ClippingFrame::good() {
    if (player_) {
        return player_->info()->error() == NULL;
    }

    return false;
}

Player *ClippingFrame::player() {
    return player_.get();
}

uint32_t ClippingFrame::frame_count() {
    return player_->info()->count();
}

void ClippingFrame::positionate_left(uint32_t frame) {
    auto key = at(frame);

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[0].x <= 0) {
        return;
    }

    key.px -= bb[0].x;
    add(key);
}

void ClippingFrame::positionate_right(uint32_t frame) {
    auto key = at(frame);

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[1].x >= player_->info()->w()) {
        return;
    }

    key.px = player_->info()->w() - (bb[1].x - bb[0].x) / 2;
    add(key);
}

void ClippingFrame::positionate_top(uint32_t frame) {
    auto key = at(frame);

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[0].y <= 0) {
        return;
    }

    key.py -= bb[0].y;
    add(key);
}

void ClippingFrame::positionate_bottom(uint32_t frame) {
    auto key = at(frame);

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[2].y >= player_->info()->h()) {
        return;
    }

    key.py = player_->info()->h() - (bb[2].y - bb[1].y) / 2;
    add(key);
}

void ClippingFrame::center_vertical(uint32_t frame) {
    auto key = at(frame);
    key.py = player_->info()->h() / 2.0;
    add(key);
}

void ClippingFrame::center_horizontal(uint32_t frame) {
    auto key = at(frame);
    key.px = player_->info()->w() / 2.0;
    add(key);
}

void ClippingFrame::normalize_scale(uint32_t frame) {
    add(at(frame).constrained(this));
}

void ClippingFrame::fit_vertical(uint32_t frame) {
    center_vertical(frame);

    auto key = at(frame);
    auto b = key.constrained(this).clipping_box(this).occupied_area();

    if (b[0].y > 0) {
        int count = (h() / static_cast<double>(b[2].y - b[0].y)) + 2;
        key.scale *= count;
        add(key);
    }

    normalize_scale(frame);
}

void ClippingFrame::fit_horizontal(uint32_t frame) {
    center_horizontal(frame);

    auto key = at(frame);
    auto b = key.constrained(this).clipping_box(this).occupied_area();

    if (b[0].y > 0) {
        int count = (w() / static_cast<double>(b[1].x - b[0].x)) + 2;
        key.scale *= count;
        add(key);
    }

    normalize_scale(frame);
}

void ClippingFrame::fit_left(uint32_t frame) {
    center_horizontal(frame);
    fit_vertical(frame);
    positionate_left(frame);
}

void ClippingFrame::fit_right(uint32_t frame) {
    center_horizontal(frame);
    fit_vertical(frame);
    positionate_right(frame);
}

void ClippingFrame::fit_top(uint32_t frame) {
    center_vertical(frame);
    fit_horizontal(frame);
    positionate_top(frame);
}

void ClippingFrame::fit_bottom(uint32_t frame) {
    center_vertical(frame);
    fit_horizontal(frame);
    positionate_bottom(frame);
}

void ClippingFrame::fit_all(uint32_t frame) {
    center_vertical(frame);
    center_vertical(frame);
    fit_vertical(frame);
    fit_horizontal(frame);
}

ClippingKey ClippingFrame::current_key() {
    return at(player_->info()->position());
}


}  // namespace vcutter

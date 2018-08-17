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

    player_.reset(new PlayerWrapper(video_path().c_str()));

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

PlayerWrapper *ClippingFrame::player() {
    return player_.get();
}

void ClippingFrame::remove_before(uint32_t frame) {
    inc_version();

    auto key_at_frame1 = at(frame);

    auto it = keys_.begin();
    while (it != keys_.end()) {
        if ((it->frame <= frame) || (!keys_.empty() && it->frame == keys_.rbegin()->frame && frame >= it->frame)) {
            it = keys_.erase(it);
            continue;
        }
        ++it;
    }

    if (frame + 1 >= player_->info()->count()) {
        frame = player_->info()->count() -2;
    }

    key_at_frame1.frame = frame;

    add(key_at_frame1);

    if (keys_.size() < 2) {
        frame += 1;
        auto key_at_frame2 = at(frame);
        add(key_at_frame2);
    }
}

uint32_t ClippingFrame::last_frame() {
    if (keys_.empty()) {
        return player_->info()->count();
    }
    return ClippingData::last_frame();
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

    key.px += player_->info()->w() - bb[1].x;
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

    key.py += player_->info()->h() - bb[2].y;
    add(key);
}

void ClippingFrame::positionate_vertical(uint32_t frame) {
    auto key = at(frame);
    key.py = player_->info()->h() / 2.0;
    add(key);
}

void ClippingFrame::positionate_horizontal(uint32_t frame) {
    auto key = at(frame);
    key.px = player_->info()->w() / 2.0;
    add(key);
}

void ClippingFrame::normalize_scale(uint32_t frame) {
    add(at(frame).constrained(this));
}

void ClippingFrame::align_left(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void ClippingFrame::align_right(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void ClippingFrame::align_top(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void ClippingFrame::align_bottom(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void ClippingFrame::align_all(uint32_t frame) {
    normalize_scale(frame);
}

ClippingKey ClippingFrame::current_key() {
    return at(player_->info()->position());
}


}  // namespace vcutter

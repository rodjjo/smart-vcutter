/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/data/json_file.h"
#include "src/clippings/clipping_data.h"

namespace vcutter {

ClippingData::ClippingData(const char *path) {
    output_w_ = 0;
    output_h_ = 0;
    load_file(path);
    version_ = 0;
}

ClippingData::ClippingData(const Json::Value * root) {
    output_w_ = 0;
    output_h_ = 0;
    load_json(*root);
    version_ = 0;
}

ClippingData::~ClippingData() {

}

void ClippingData::load_json(const Json::Value & root) {
    video_path_ = root["video_path"].asString();

    if (root.isMember("width")) {
        output_w_ = root["width"].asInt();
    }

    if (root.isMember("height")) {
        output_h_ = root["height"].asInt();
    }

    if (root.isMember("keys")) {
        auto & k = root["keys"];
        for (Json::Value::ArrayIndex i = 0; i < k.size(); ++i)  {
            add(ClippingKey(k[i]));
        }
    }
}

Json::Value ClippingData::serialize() {
    Json::Value data;

    data["video_path"] = video_path_;
    data["width"] = output_w_;
    data["height"] = output_h_;

    Json::Value keys;

    for (auto & k : keys_) {
        keys.append(k.serialize());
    }

    data["keys"] = keys;

    return data;
}

void ClippingData::load_file(const char *path) {
    if (!*path) {
        return;
    }

    JsonFile jsf(path);

    if (!jsf.loaded()) {
        return;
    }

    load_json(jsf["ClippingData"]);
    saved_path_ = path;
}

void ClippingData::save(const char *path) {
    JsonFile jsf(path, false, false);
    jsf["ClippingData"] = serialize();
    jsf.save();
    saved_path_ = path;
}

std::string ClippingData::saved_path() {
    return saved_path_;
}

void ClippingData::inc_version() {
    ++version_;
}

void ClippingData::add(const ClippingKey & key) {
    inc_version();

    auto it = keys_.begin();

    while (it != keys_.end()) {
        if (it->frame == key.frame) {
            *it = key;
            it->computed_ = false;
            return;
        } else if (it->frame > key.frame) {
            auto nk = keys_.insert(it, key);
            nk->computed_ = false;
            return;
        }
        ++it;
    }

    keys_.push_back(key);
    keys_.rbegin()->computed_ = false;
}

uint32_t ClippingData::w() {
    return output_w_;
}

uint32_t ClippingData::h() {
    return output_h_;
}

uint32_t ClippingData::req_buffer_size() {
    return output_w_ * output_h_ * 3;
}

void ClippingData::w(uint32_t value) {
    inc_version();
    output_w_ = value;
}

void ClippingData::h(uint32_t value) {
    inc_version();
    output_h_ = value;
}

const std::list<ClippingKey> ClippingData::keys() const {
    return keys_;
}

ClippingKey ClippingData::compute_interpolation(uint32_t frame) {
    bool first_found = false;
    ClippingKey left, right;

    auto begin = keys_.begin();
    auto end = keys_.end();

    left = *begin;
    right = *begin;

    while (begin != end)  {
        if (begin->frame <= frame) {
            left = *begin;
            right = *begin;
            first_found = true;
        }

        if (begin->frame >= frame) {
            right = *begin;
            if (!first_found) {
                left = *begin;
                first_found = true;
            }
            break;
        }
        ++begin;
    }

    if (left.frame == frame) {
        return left;
    }

    if (right.frame == frame) {
        return right;
    }

    ClippingKey current = left;
    current.frame = frame;

    bool clockwise = true;

    if (left.frame < right.frame) {
        float frame_diff = frame - left.frame;

        double interpolation = (1.0f / static_cast<double>(right.frame - left.frame)) * frame_diff;

        float difx = (right.px - left.px);
        float dify = (right.py - left.py);
        float difs = (right.scale - left.scale);
        double difa = right.angle() - left.angle();

        if (difa < 0) {
            clockwise = (difa + 360) <= 180;
        } else {
            clockwise = difa <= 180;
        }

        if (!clockwise) {
            difa = -difa;
        }
        if (difa < 0) {
            difa += 360;
        }

        current.px = left.px + difx * interpolation;
        current.py = left.py + dify * interpolation;
        current.scale = left.scale + difs * interpolation;
        current.angle(left.angle() + (difa * interpolation * (clockwise ? 1 : -1)));
    }

    current.computed_ = true;
    return current;
}

ClippingKey ClippingData::at(uint32_t frame) {
    if (!keys_.empty()) {
        return compute_interpolation(frame);
    }

    ClippingKey result;

    result.scale = 1.0;
    result.angle(0);
    result.frame = frame;
    result.px = default_w() / 2;
    result.py = default_h() / 2;

    return result;
}

void ClippingData::remove(uint32_t frame) {
     if (keys_.size() < 1)  {
        return;
    }

    inc_version();

    for(auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->frame == frame) {
            keys_.erase(it);
            return;
        }
    }
}

void ClippingData::define_start(uint32_t frame) {
    inc_version();

    auto key_at_frame1 = at(frame);

    auto it = keys_.begin();
    while (it != keys_.end()) {
        if ((it->frame <= frame) || (it->frame == keys_.rbegin()->frame && frame > it->frame)) {
            it = keys_.erase(it);
            continue;
        }
        ++it;
    }

    if (frame + 1 >= frame_count()) {
        frame = frame_count() -2;
    }

    key_at_frame1.frame = frame;

    add(key_at_frame1);
}

void ClippingData::define_end(uint32_t frame) {
    inc_version();

    auto key_at_frame1 = at(frame);

    auto it = keys_.begin();
    while (it != keys_.end()) {
        if (it->frame >= frame || (it->frame == keys_.begin()->frame && frame < it->frame)) {
            it = keys_.erase(it);
            continue;
        }
        ++it;
    }

    if (frame - 1 < 0) {
        frame = 1;
    }

    key_at_frame1.frame = frame;

    add(key_at_frame1);
}

void ClippingData::remove_all(uint32_t frame_to_keep) {
    inc_version();
    auto key = at(frame_to_keep);
    keys_.clear();
    add(key);
}

void ClippingData::remove_all() {
    inc_version();
    keys_.clear();
}

uint32_t ClippingData::first_frame() {
    if (keys_.empty()) {
        return 1;
    }

    return keys_.begin()->frame;
}

uint32_t ClippingData::last_frame() {
    if (keys_.empty()) {
        return frame_count();
    }

    return keys_.rbegin()->frame;
}

void ClippingData::wh(uint32_t w, uint32_t h) {
    this->w(w);
    this->h(h);
}

ClippingKey ClippingData::at_index(uint32_t index) {
    auto iterator = keys_.begin();
    std::advance(iterator, index);
    return *iterator;
}

int ClippingData::find_index(uint32_t frame) {
    int index = 0;
    for (const auto & c : keys_) {
        if (c.frame == frame) {
            return index;
        }
        ++index;
    }

    return -1;
}

std::string ClippingData::video_path() {
    return video_path_;
}

void ClippingData::video_path(const char *value) {
    video_path_ = value;
}

uint64_t ClippingData::version() {
    return version_;
}

}  // namespace vcutter

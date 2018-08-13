/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/data/json_file.h"
#include "src/clippings/clipping.h"

namespace vcutter {

Clipping::Clipping(const char *path, bool path_is_video) {
    output_w_ = 0;
    output_h_ = 0;
    if (path_is_video) {
        video_open(path);
    } else {
        load_file(path);
    }
    version_ = 0;
}

Clipping::Clipping(const Json::Value * root) {
    output_w_ = 0;
    output_h_ = 0;
    load_json(*root);
    version_ = 0;
}

void Clipping::video_open(const char *path) {
    if (!*path) {
        return;
    }

    video_path_ = path;
    player_.reset(new PlayerWrapper(path));

    if (!good()){
        return;
    }

    if (!output_w_) {
        output_w_ = player_->info()->w();
    }

    if (!output_h_) {
        output_w_ = player_->info()->h();
    }
}

void Clipping::load_json(const Json::Value & root) {
    auto path = root["video_path"].asString();

    video_open(path.c_str());

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

Json::Value Clipping::serialize() {
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

void Clipping::load_file(const char *path) {
    JsonFile jsf(path);

    if (!jsf.loaded()) {
        return;
    }

    load_json(jsf["clipping"]);
}

void Clipping::save(const char *path) {
    JsonFile jsf(path, false, false);
    jsf["clipping"] = serialize();
    jsf.save();
}

bool Clipping::good() {
    if (player_) {
        return player_->info()->error() == NULL;
    }

    return false;
}

void Clipping::inc_version() {
    ++version_;
}

void Clipping::add(const ClippingKey & key) {
    double last_angle = key.angle();

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

uint32_t Clipping::w() {
    return output_w_;
}

uint32_t Clipping::h() {
    return output_h_;
}

PlayerWrapper *Clipping::player() {
    return player_.get();
}

uint32_t Clipping::req_buffer_size() {
    return output_w_ * output_h_ * 3;
}

void Clipping::w(uint32_t value) {
    if (value > player_->info()->w()) {
        return;
    }
    inc_version();
    output_w_ = value;
}

void Clipping::h(uint32_t value) {
    if (value > player_->info()->h()) {
        return;
    }
    inc_version();
    output_h_ = value;
}

const std::list<ClippingKey> Clipping::keys() const {
    return keys_;
}

ClippingKey Clipping::compute_interpolation(uint32_t frame) {
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

ClippingKey Clipping::operator[] (uint32_t frame) {
    if (!keys_.empty()) {
        return compute_interpolation(frame);
    }

    ClippingKey result;

    result.scale = 1.0;
    result.angle(0);
    result.frame = frame;
    result.px = player_->info()->w() / 2;
    result.py = player_->info()->h() / 2;

    return result;
}

}  // namespace vcutter

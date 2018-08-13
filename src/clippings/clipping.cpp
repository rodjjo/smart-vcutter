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
        video_path_ = path;
        player_.reset(new PlayerWrapper(path));
        if (good()){
            after_video_open();
        }
    } else {
        load(path);
    }
    version_ = 0;
}

Clipping::Clipping(const Json::Value & root) {
    output_w_ = 0;
    output_h_ = 0;
    load(root);
    version_ = 0;
}

void Clipping::after_video_open() {
    if (!output_w_) {
        output_w_ = player_->info()->w();
    }

    if (!output_h_) {
        output_w_ = player_->info()->h();
    }
}

void Clipping::load(const Json::Value & root) {
    auto path = root["video_path"].asString();

    if (path.empty()) {
        return;
    }

    video_path_ = path;
    player_.reset(new PlayerWrapper(path));
    if (!good()){
        return;
    }

    after_video_open();

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

    auto & keys = data["keys"];

    for (auto & k : keys_) {
        keys.append(k.serialize());
    }

    return data;
}

void Clipping::load(const char *path) {
    JsonFile jsf(path);

    if (!jsf.loaded()) {
        return;
    }

    load(jsf["clipping"]);
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


}  // namespace vcutter

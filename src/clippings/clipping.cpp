/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include "src/data/json_file.h"
#include "src/clippings/clipping.h"

namespace vcutter {

namespace tmp { // temporay namespace (will be removed soon)

void adjust_regions_of_interest(double scale, const cv::Mat& source, const cv::Mat& target, cv::Rect *roi_in,  cv::Rect *roi_out) {
  if (roi_in->x < 0) {
    roi_in->width += roi_in->x;
    int x = scale * -roi_in->x;
    roi_out->x = x;
    roi_out->width -= x;
    roi_in->x = 0;
  }

  if (roi_in->y < 0) {
    roi_in->height += roi_in->y;
    int y = scale * -roi_in->y;
    roi_out->y = y;
    roi_out->height -= y;
    roi_in->y = 0;
  }

  if (roi_in->x + roi_in->width > source.cols) {
    int pass_x = roi_in->x + roi_in->width - source.cols;
    roi_in->width -= pass_x;
    pass_x *= scale;
    roi_out->width -= pass_x;
  }

  if (roi_in->y + roi_in->height > source.rows) {
    int pass_y = roi_in->y + roi_in->height - source.rows;
    roi_in->height -= pass_y;
    pass_y *= scale;
    roi_out->height -= pass_y;
  }
}

void copy_center(cv::Mat& source, cv::Mat& target) {
  int dx = source.cols - target.cols;
  int dy = source.rows - target.rows;
  cv::Rect roi_in(dx / 2, dy / 2, target.cols, target.rows);
  cv::Rect roi_out(0, 0, target.cols, target.rows);
  tmp::adjust_regions_of_interest(1.0, source, target, &roi_in, &roi_out);
  if (roi_in.width < 1 || roi_in.height < 1 || roi_out.width < 1 || roi_out.height < 1)
      return;
  cv::Mat roi_img_in(source(roi_in));
  cv::Mat roi_img_out(target(roi_out));
  roi_img_in.copyTo(roi_img_out);
}

}  // namespace

Clipping::Clipping(const char *path, bool path_is_video) {
    clear_reference();
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
    clear_reference();

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

void Clipping::remove(uint32_t frame) {
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

void Clipping::remove_before(uint32_t frame) {
    inc_version();

    auto key_at_frame1 = (*this)[frame];

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
        auto key_at_frame2 = (*this)[frame];
        add(key_at_frame2);
    }
}

void Clipping::remove_after(uint32_t frame) {
    inc_version();

    auto key_at_frame1 = (*this)[frame];

    auto it = keys_.begin();
    while (it != keys_.end()) {
        if (it->frame >= frame || (!keys_.empty() && it->frame == keys_.begin()->frame && frame <= it->frame)) {
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

    if (keys_.size() < 2) {
        --frame;
        auto key_at_frame2 = (*this)[frame];
        add(key_at_frame2);
    }
}

void Clipping::remove_others(uint32_t frame_to_keep) {
    inc_version();
    auto key = (*this)[frame_to_keep];
    keys_.clear();
    add(key);
}

void Clipping::remove_all() {
    inc_version();
    keys_.clear();
}

uint32_t Clipping::first_frame() {
    if (keys_.empty()) {
        return 1;
    }

    return keys_.begin()->frame;
}

uint32_t Clipping::last_frame() {
    if (keys_.empty()) {
        return player_->info()->count();
    }
    return keys_.rbegin()->frame;
}

void Clipping::wh(uint32_t w, uint32_t h) {
    this->w(w);
    this->h(h);
}

ClippingKey Clipping::at_index(uint32_t index) {
    auto iterator = keys_.begin();
    std::advance(iterator, index);
    return *iterator;
}

int Clipping::find_index(uint32_t frame) {
    int index = 0;
    for (const auto & c : keys_) {
        if (c.frame == frame) {
            return index;
        }
        ++index;
    }

    return -1;
}

std::string Clipping::video_path() {
    return video_path_;
}

uint64_t Clipping::version() {
    return version_;
}

void Clipping::positionate_left(uint32_t frame) {
    auto key = (*this)[frame];

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[0].x <= 0) {
        return;
    }

    key.px -= bb[0].x;
    add(key);
}

void Clipping::positionate_right(uint32_t frame) {
    auto key = (*this)[frame];

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[1].x >= player_->info()->w()) {
        return;
    }

    key.px += player_->info()->w() - bb[1].x;
    add(key);
}

void Clipping::positionate_top(uint32_t frame) {
    auto key = (*this)[frame];

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[0].y <= 0) {
        return;
    }

    key.py -= bb[0].y;
    add(key);
}

void Clipping::positionate_bottom(uint32_t frame) {
    auto key = (*this)[frame];

    auto bb = key.constrained(this).clipping_box(this).occupied_area();

    if (bb[2].y >= player_->info()->h()) {
        return;
    }

    key.py += player_->info()->h() - bb[2].y;
    add(key);
}

void Clipping::positionate_vertical(uint32_t frame) {
    auto key = (*this)[frame];
    key.py = player_->info()->h() / 2.0;
    add(key);
}

void Clipping::positionate_horizontal(uint32_t frame) {
    auto key = (*this)[frame];
    key.px = player_->info()->w() / 2.0;
    add(key);
}

void Clipping::normalize_scale(uint32_t frame) {
    add((*this)[frame].constrained(this));
}

void Clipping::align_left(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void Clipping::align_right(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void Clipping::align_top(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void Clipping::align_bottom(uint32_t frame) {
    normalize_scale(frame);  // TODO(Rodrigo): Implement this operation
}

void Clipping::align_all(uint32_t frame) {
    normalize_scale(frame);
}

ClippingKey Clipping::current_key() {
    return (*this)[player_->info()->position()];
}

void Clipping::render(ClippingKey key, uint8_t *source_buffer, uint32_t target_w, uint32_t target_h, uint8_t *buffer, bool transparent) {
    // TODO(Rodrigo Simplify this logic):
    int source_w = player_->info()->w();
    int source_h = player_->info()->h();

    key = key.constrained(this);

    box_t box =  key.clipping_box(this);
    box_t bbox = box.occupied_area();
    int bbox_w = bbox[1].x - bbox[0].x;
    int bbox_h = bbox[2].y - bbox[0].y;

    assert(bbox_w >= 15); // TODO(Rodrigo): garantee on tests and remove these assertions
    assert(bbox_h >= 15);

    assert(bbox[0].y >= 0);
    assert(bbox[0].x >= 0);

    assert(bbox[0].y + bbox_h < source_h);
    assert(bbox[0].x + bbox_w < source_w);

    cv::Mat frame(source_h, source_w, CV_8UC3, source_buffer);
    cv::Mat output(target_h, target_w, CV_8UC3, buffer);
    if (key.angle() == 0) {
        cv::Rect roi_input(bbox[0].x, bbox[0].y, bbox_w, bbox_h);
        cv::Mat roi_img_in(frame(roi_input));

        if (!transparent) {
            cv::resize(roi_img_in, output, output.size(), CV_INTER_LANCZOS4);
        } else {
            cv::Mat temp_output(target_h, target_w, CV_8UC3);
            cv::resize(roi_img_in, temp_output, temp_output.size(), CV_INTER_LANCZOS4);
            cv::addWeighted(temp_output, 0.5, output, 0.5, 0.0, output);
        }

        return;
    }

    int hypo = sqrt(bbox_w * bbox_w + bbox_h * bbox_h);
    cv::Mat rotated = cv::Mat(hypo + 2, hypo + 2, CV_8UC3);

    int half_w = bbox_w / 2;
    int half_h = bbox_h / 2;
    cv::Rect roi_input(key.px - half_w, key.py - half_h, bbox_w, bbox_h);

    half_w = rotated.cols / 2;
    half_h = rotated.rows / 2;
    cv::Rect roi_output(half_w - bbox_w / 2, half_h - bbox_h / 2, bbox_w, bbox_h);

    cv::Mat roi_img_in(frame(roi_input));
    cv::Mat roi_img_out(rotated(roi_output));
    roi_img_in.copyTo(roi_img_out);

    cv::Point2f src_center(half_w, half_h);

    cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, key.angle() - 360, 1.0);
    cv::Mat temp;
    cv::warpAffine(rotated, temp, rot_mat, rotated.size(), CV_INTER_LANCZOS4);
    rotated = cv::Mat(target_h * key.scale, target_w * key.scale, CV_8UC3);

    tmp::copy_center(temp, rotated);

    if (transparent) {
        cv::Mat temp_output(target_h, target_w, CV_8UC3);
        cv::resize(rotated, temp_output, temp_output.size(), CV_INTER_LANCZOS4);
        cv::addWeighted(temp_output, 0.5, output, 0.5, 0.0, output);
    } else {
        cv::resize(rotated, output, output.size(), CV_INTER_LANCZOS4);
    }
}

void Clipping::render(ClippingKey key, uint8_t *player_buffer, uint8_t *buffer) {
    render(
        key,
        player_buffer,
        output_w_,
        output_h_,
        buffer,
        false);
}

void Clipping::render_transparent(ClippingKey key, uint8_t *player_buffer, uint8_t *buffer) {
    render(
        key,
        player_buffer,
        output_w_,
        output_h_,
        buffer,
        true);
}

void Clipping::render(ClippingKey key, uint32_t target_w, uint32_t target_h, uint8_t *buffer) {
    render(
        key,
        player_->info()->buffer(),
        target_w,
        target_h,
        buffer,
        false);
}

void Clipping::render(ClippingKey key, uint8_t *buffer) {
    render(
        key,
        player_->info()->buffer(),
        output_w_,
        output_h_,
        buffer,
        false);
}

bool Clipping::get_reference(int *frame, float *rx1, float *ry1, float *rx2, float *ry2) {
    if (!has_ref_) {
        return false;
    }

    for (auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->frame == ref_frame_) {
            *frame = ref_frame_;
            *rx1 = rx1_;
            *ry1 = ry1_;
            *rx2 = rx2_;
            *ry2 = ry2_;
            return true;
        }
    }
    has_ref_ = false;
    return false;
}

bool Clipping::get_reference_frame(int *frame) {
    if (!has_ref_) {
        return false;
    }
    for (auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->frame == ref_frame_) {
            *frame = ref_frame_;
            return true;
        }
    }
    has_ref_ = false;
    return false;
}

bool Clipping::has_ref() {
    if (has_ref_) {
        for (auto it = keys_.begin(); it != keys_.end(); ++it) {
            if (it->frame == ref_frame_) {
                return true;
            }
        }
        has_ref_ = false;
    }
    return false;
}

void Clipping::set_reference(int frame, float rx1, float ry1, float rx2, float ry2) {
    rx1_ = rx1;
    ry1_ = ry1;
    rx2_ = rx2;
    ry2_ = ry2;
    ref_frame_ = frame;
    has_ref_ = true;
}

void Clipping::clear_reference() {
    has_ref_ = false;
}

}  // namespace vcutter

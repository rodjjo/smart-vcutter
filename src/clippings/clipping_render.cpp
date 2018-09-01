/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include "src/clippings/clipping_render.h"

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

}  // namespace tmp

ClippingRender::ClippingRender(const char *path, bool path_is_video) : ClippingFrame(path, path_is_video) {
}

ClippingRender::ClippingRender(const Json::Value * root) : ClippingFrame(root) {
}

void ClippingRender::render(ClippingKey key, uint8_t *source_buffer, uint32_t target_w, uint32_t target_h, uint8_t *buffer) {
    // TODO(Rodrigo Simplify this logic):
    int source_w = player()->info()->w();
    int source_h = player()->info()->h();

    key = key.constrained(this);

    box_t bbox = key.clipping_box(this).occupied_area();

    int bbox_w = bbox[1].x - bbox[0].x;
    int bbox_h = bbox[2].y - bbox[0].y;

    cv::Mat frame(source_h, source_w, CV_8UC3, source_buffer);
    cv::Mat output(target_h, target_w, CV_8UC3, buffer);
    if (key.angle() == 0) {
        cv::Rect roi_input(bbox[0].x, bbox[0].y, bbox_w, bbox_h);
        cv::Mat roi_img_in(frame(roi_input));

        cv::resize(roi_img_in, output, output.size(), CV_INTER_LANCZOS4);
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

    cv::resize(rotated, output, output.size(), CV_INTER_LANCZOS4);
}

void ClippingRender::render(ClippingKey key, uint8_t *player_buffer, uint8_t *buffer) {
    render(
        key,
        player_buffer,
        w(),
        h(),
        buffer);
}

void ClippingRender::render(ClippingKey key, uint32_t target_w, uint32_t target_h, uint8_t *buffer) {
    render(
        key,
        player()->info()->buffer(),
        target_w,
        target_h,
        buffer);
}

void ClippingRender::render(ClippingKey key, uint8_t *buffer) {
    render(
        key,
        player()->info()->buffer(),
        w(),
        h(),
        buffer);
}

std::shared_ptr<ClippingRender> ClippingRender::clone() {
  std::shared_ptr<ClippingRender> clipping(new ClippingRender(video_path().c_str(), true));
  clipping->wh(w(), h());

  for (const auto & k : keys()) {
    clipping->add(k);
  }

  return clipping;
}

}  // namespace vcutter

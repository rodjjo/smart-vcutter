/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cmath>
#include <cassert>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>

#include "smart-vcutter/clippings/clipping_painter.h"

namespace vcutter {

box_t clipping_box(const clipping_key_t& interpolated_clipping, unsigned int target_w, unsigned int target_h) {
    box_t result;
    result[0].x = 0;
    result[0].y = 0;
    result[1].x = target_w;
    result[1].y = 0;
    result[2].x = target_w;
    result[2].y = target_h;
    result[3].x = 0;
    result[3].y = target_h;
    float half_x = target_w / 2.0;
    float half_y = target_h / 2.0;

    result.translate(-half_x, -half_y);
    result.rotate(interpolated_clipping.angle);
    result.scale(interpolated_clipping.scale);
    result.translate(interpolated_clipping.px, interpolated_clipping.py);
    result.trunc_precision();

    return result;
}

clipping_key_t limit_scale(float width, float height, float fw, float fh, const clipping_key_t& interpolated_clipping) {
    clipping_key_t result = interpolated_clipping;
    box_t area = clipping_box(result, fw, fh).occupied_area();

    point_t center(area.center());

    point_t lt = area.left_top_violation(width, height);
    point_t rb = area.right_bottom_violation(width, height);

    point_t sz = area.size();

    int xpass = lt.x > rb.x ? lt.x : rb.x;
    int ypass = lt.y > rb.y ? lt.y : rb.y;

    if (xpass == 0 && ypass == 0) {
       return result;
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

    result.scale *= (scalex < scaley ? scalex : scaley);

    return result;
}

clipping_key_t adjust_bounds(
    const clipping_key_t& interpolated_clipping,
    unsigned int target_w,
    unsigned int target_h,
    unsigned int max_x,
    unsigned int max_y
) {
    clipping_key_t result = interpolated_clipping;

    while (result.angle < 0) {
        result.angle += 360;
    }

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

    return limit_scale(max_x - 1, max_y - 1, target_w, target_h, result);
}

clipping_key_t compute_interpolation(
    unsigned int current_frame,
    clipping_key_list_t::const_iterator begin,
    clipping_key_list_t::const_iterator end,
    bool *computed
) {
    if (computed) {
        *computed = true;
    }

    bool first_found = false;
    clipping_key_t left, right, current;

    left = *begin;
    right = *begin;
    while (begin != end)  {
        if (begin->frame <= current_frame) {
            left = *begin;
            right = *begin;
            first_found = true;
        }

        if (begin->frame >= current_frame) {
            right = *begin;
            if (!first_found) {
                left = *begin;
                first_found = true;
            }
            break;
        }
        ++begin;
    }

    if (computed) {
        *computed = left.frame != current_frame && right.frame != current_frame;
    }

    current = left;
    current.frame = current_frame;

    bool clockwise = true;

    if (left.frame < right.frame) {
        float frame_diff = current_frame - left.frame;

        double interpolation = (1.0f / static_cast<double>(right.frame - left.frame)) * frame_diff;

        float difx = (right.px - left.px);
        float dify = (right.py - left.py);
        float difs = (right.scale - left.scale);
        float difa = right.angle - left.angle;

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
        current.angle = normalize_angle(left.angle + (difa * interpolation * (clockwise ? 1 : -1)));
    }

    return current;
}


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
  adjust_regions_of_interest(1.0, source, target, &roi_in, &roi_out);
  if (roi_in.width < 1 || roi_in.height < 1 || roi_out.width < 1 || roi_out.height < 1)
      return;
  cv::Mat roi_img_in(source(roi_in));
  cv::Mat roi_img_out(target(roi_out));
  roi_img_in.copyTo(roi_img_out);
}

void paint_clipping(
    unsigned char *souce_buffer,
    unsigned int source_w,
    unsigned int source_h,
    const clipping_key_t& interpolated_clipping,
    unsigned int target_w,
    unsigned int target_h,
    unsigned char *target_buffer,
    bool transparent
) {
    clipping_key_t safe_clipping = adjust_bounds(interpolated_clipping, target_w, target_h, source_w, source_h);

    box_t box = clipping_box(safe_clipping, target_w, target_h);
    box_t bbox = box.occupied_area();
    int bbox_w = bbox[1].x - bbox[0].x;
    int bbox_h = bbox[2].y - bbox[0].y;

    assert(bbox_w >= 15);
    assert(bbox_h >= 15);

    assert(bbox[0].y >= 0);
    assert(bbox[0].x >= 0);

    assert(bbox[0].y + bbox_h < source_h);
    assert(bbox[0].x + bbox_w < source_w);

    cv::Mat frame(source_h, source_w, CV_8UC3, souce_buffer);
    cv::Mat output(target_h, target_w, CV_8UC3, target_buffer);
    if (safe_clipping.angle == 0) {
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

    int hypo =  sqrt(bbox_w * bbox_w + bbox_h * bbox_h);
    cv::Mat rotated = cv::Mat(hypo + 2, hypo + 2, CV_8UC3);

    int half_w = bbox_w / 2;
    int half_h = bbox_h / 2;
    cv::Rect roi_input(safe_clipping.px - half_w, safe_clipping.py - half_h, bbox_w, bbox_h);

    half_w = rotated.cols / 2;
    half_h = rotated.rows / 2;
    cv::Rect roi_output(half_w - bbox_w / 2, half_h - bbox_h / 2, bbox_w, bbox_h);

    cv::Mat roi_img_in(frame(roi_input));
    cv::Mat roi_img_out(rotated(roi_output));
    roi_img_in.copyTo(roi_img_out);

    cv::Point2f src_center(half_w, half_h);

    cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, safe_clipping.angle - 360, 1.0);
    cv::Mat temp;
    cv::warpAffine(rotated, temp, rot_mat, rotated.size(), CV_INTER_LANCZOS4);
    rotated = cv::Mat(target_h * safe_clipping.scale, target_w * safe_clipping.scale, CV_8UC3);
    copy_center(temp, rotated);
    if (transparent) {
        cv::Mat temp_output(target_h, target_w, CV_8UC3);
        cv::resize(rotated, temp_output, temp_output.size(), CV_INTER_LANCZOS4);
        cv::addWeighted(temp_output, 0.5, output, 0.5, 0.0, output);
    } else {
        cv::resize(rotated, output, output.size(), CV_INTER_LANCZOS4);
    }
}

clipping_key_t magic_tool(
    clipping_key_t source,
    unsigned int target_w,
    unsigned int target_h,
    unsigned int max_w,
    unsigned int max_h,
    float source_rx1,
    float source_ry1,
    float source_rx2,
    float source_ry2,
    float curr_rx1,
    float curr_ry1,
    float curr_rx2,
    float curr_ry2,
    bool should_rotate,
    bool should_scale,
    bool should_positionate_x,
    bool should_positionate_y
) {
    clipping_key_t result = source;
    if (!(should_rotate || should_scale || should_positionate_x || should_positionate_y)) {
        return result;
    }

    double scale = 1.0;
    if (should_scale) {
        double source_distance = point_t(source_rx1, source_ry1).distance_to(source_rx2, source_ry2);
        double target_distance = point_t(curr_rx1, curr_ry1).distance_to(curr_rx2, curr_ry2);

        if (source_distance != 0) {
            scale = target_distance / source_distance;
        }
        source.scale *= scale;
    }

    if (should_positionate_x) {
        float dx = (curr_rx1 - source_rx1) * scale;
        source.px += dx;
    }

    if (should_positionate_y) {
        float dy = (curr_ry1 - source_ry1) * scale;
        source.py += dy;
    }

    if (should_rotate) {
        float source_angle = point_t(source_rx2 - source_rx1, source_ry2  - source_ry1).angle_0_360();
        float target_angle = point_t(curr_rx2 - curr_rx1, curr_ry2 - curr_ry1).angle_0_360();
        float diff = target_angle - source_angle;
        if (diff) {
            source.angle = normalize_angle(source.angle + diff);
        }
    }

    source = adjust_bounds(source, target_w, target_h, max_w, max_h);
    box_t bb = clipping_box(source, target_w, target_h).occupied_area();
    if (bb[1].x - bb[0].x > 15 && bb[2].y - bb[0].y > 15) {
        result.scale = source.scale;
    }

    if (source.px >= 1 && source.py >= 1 && source.px + 1 < max_w && source.py + 1 < max_h) {
        result.px = source.px;
        result.py = source.py;
    }

    result.angle = source.angle;

    return result;
}

}  // namespace vcutter
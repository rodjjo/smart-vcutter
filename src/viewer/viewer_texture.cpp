/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <GL/gl.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <FL/Fl.H>
#include <FL/gl.h>

#include "src/viewer/viewer_texture.h"


namespace vcutter {


ViewerTexture::ViewerTexture() {
    texture_id_ = 0;
    texture_w_ = 0;
    texture_h_ = 0;
    resize_texture_ = false;
    rgba_ = false;
    buffer_w_ = 0;
    buffer_h_ = 0;
    view_w_ = 0;
    view_h_ = 0;
}

ViewerTexture::ViewerTexture(const uint8_t *buffer, uint32_t w, uint32_t h, bool resize_texture, bool rgba) {
    texture_id_ = 0;
    texture_w_ = 0;
    texture_h_ = 0;
    resize_texture_ = false;
    rgba_ = false;
    buffer_w_ = 0;
    buffer_h_ = 0;
    view_w_ = 0;
    view_h_ = 0;
    update(buffer, w, h, resize_texture, rgba);
}

ViewerTexture::~ViewerTexture() {
    if (texture_id_) {
        gl_start();
        glDeleteTextures(1, &texture_id_);
        gl_finish();
    }
}

void ViewerTexture::update(const uint8_t *buffer, uint32_t w, uint32_t h, bool resize_texture, bool rgba) {
    auto buffer_size = w * h * (rgba ? 4 : 3);
    buffer_.reset(new uint8_t[buffer_size], [](const uint8_t *b) { delete[] b;});
    buffer_w_ = w;
    buffer_h_ = h;
    resize_texture_ = resize_texture;
    rgba_ = rgba;
    memcpy(buffer_.get(), buffer, buffer_size);
}

void ViewerTexture::draw(const viewport_t &vp, float x, float y, float zoom) {
    update_texture(vp, NULL, 0, 0, resize_texture_, rgba_);

    if (!texture_w_ || !texture_h_ || !texture_id_) {
        return;
    }

    x = x * (2.0f / vp[2]) - 1.0f;
    y = (vp[3] - y) * (2.0f / vp[3]) - 1.0f;

    float tw = (2.0 / vp[2]) * (texture_w_ * zoom);
    float th = (2.0 / vp[3]) * (texture_h_ * zoom);

    if (rgba_) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glBegin (GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(x - tw, y + th);
    glTexCoord2d(1.0, 0.0); glVertex2d(x + tw, y + th);
    glTexCoord2d(1.0, 1.0); glVertex2d(x + tw, y - th);
    glTexCoord2d(0.0, 1.0); glVertex2d(x - tw, y - th);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    if (rgba_) {
        glDisable(GL_BLEND);
    }
}

void ViewerTexture::draw(const viewport_t &vp, const uint8_t *buffer, uint32_t w, uint32_t h, bool resize_texture, bool rgba) {
    update_texture(vp, buffer, w, h, resize_texture, rgba);

    if (!texture_w_ || !texture_h_ || !texture_id_) {
        return;
    }

    float coord_w = ((2.0 / vp[2]) * texture_w_) / 2.0;
    float coord_h = ((2.0 / vp[3]) * texture_h_) / 2.0;

    if (rgba) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glBegin (GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(-coord_w, coord_h);
    glTexCoord2d(1.0, 0.0); glVertex2d(coord_w, coord_h);
    glTexCoord2d(1.0, 1.0); glVertex2d(coord_w, -coord_h);
    glTexCoord2d(0.0, 1.0); glVertex2d(-coord_w, -coord_h);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    if (rgba_) {
        glDisable(GL_BLEND);
    }
}

void ViewerTexture::draw(const viewport_t &vp, uint32_t vw, uint32_t vh, box_t texture_coords, box_t view_coords, float alpha) {
    update_texture(vp, NULL, 0, 0, resize_texture_, rgba_);

    if (!texture_w_ || !texture_h_ || !texture_id_) {
        return;
    }

    for (char i = 0; i < 4; ++i) {
        view_coords[i] = vp.frame_to_screen_coords(vw, vh, view_coords[i]);
        view_coords[i].x = view_coords[i].x * (2.0f / vp[2]) - 1.0f;
        view_coords[i].y = (vp[3] - view_coords[i].y) * (2.0f / vp[3]) - 1.0f;
        texture_coords[i].x = (1.0f / vw) * texture_coords[i].x;
        texture_coords[i].y = (1.0f / vh) * texture_coords[i].y;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, alpha);

    glBegin (GL_QUADS);
    for (char i = 0; i < 4; ++i) {
        glTexCoord2d(texture_coords[i].x, texture_coords[i].y);
        glVertex2d(view_coords[i].x, view_coords[i].y);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
}


void ViewerTexture::update_texture(const viewport_t &vp, const uint8_t* buffer, uint32_t w, uint32_t h, bool resize_texture, bool rgba) {
    if (!buffer && !buffer_) {
        return;
    }

    if (!buffer) {
        buffer = buffer_.get();
        w = buffer_w_;
        h = buffer_h_;
        resize_texture = resize_texture_;
        rgba = rgba_;
    } else {
        resize_texture_ = resize_texture;
        rgba_ = rgba;
    }

    if (texture_id_ == 0 || (resize_texture && (static_cast<uint32_t>(vp[2]) != view_w_ || static_cast<uint32_t>(vp[3]) != view_h_)) || w != texture_w_ || h != texture_h_) {
        if (texture_id_) {
            glDeleteTextures(1, &texture_id_);
        }

        glGenTextures(1, &texture_id_);

        if (!texture_id_) {
            buffer_.reset();
            return;
        }

        view_w_ = vp[2];
        view_h_ = vp[3];
        texture_w_ = w;
        texture_h_ = h;

        if (resize_texture) {
            vp.fit(&texture_w_, &texture_h_);
        }
    }

    glBindTexture(GL_TEXTURE_2D, texture_id_);

    if (texture_w_ % 4 == 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if (w == texture_w_ && h == texture_h_) {
        glTexImage2D(GL_TEXTURE_2D, 0, rgba ? GL_RGBA : GL_RGB , texture_w_, texture_h_, 0, rgba ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, buffer);
    } else {
        cv::Mat texture(texture_h_, texture_w_, CV_8UC3);
        cv::Mat source(h, w, rgba ? CV_8UC4 : CV_8UC3, const_cast<unsigned char *>(buffer));
        cv::resize(source, texture, texture.size(), CV_INTER_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, rgba ? GL_RGBA : GL_RGB, texture_w_, texture_h_, 0, rgba ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texture.data);
    }

    if (buffer_) {
        buffer_.reset();
    }
}

}  // namespace vcutter

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/viewer/miniature_viewer.h"

namespace vcutter {


MiniatureViewer::MiniatureViewer(uint32_t x, uint32_t y, uint32_t w, uint32_t h) : BufferViewer(this, this, x, y, w, h) {
    invalidate();
    viewer_texture_.reset(new ViewerTexture());
}

MiniatureViewer::~MiniatureViewer() {
}

void MiniatureViewer::invalidate() {
    clipping_ = NULL;
    miniature_buffer_w_ = 0;
    miniature_buffer_h_ = 0;
    buffer_size_ = 0;
    modified_ = true;
    clipping_buffer_.reset();
}

void MiniatureViewer::viewer_draw(BufferViewer *viewer, bool *handled, const uint8_t* buffer, uint32_t w, uint32_t h) {
    if (!clipping_) {
        return;
    }

    if (clipping_->player()->is_playing()) {
        return;
    }

    if (modified_) {
        viewer_texture_->draw(viewer->view_port(), buffer, w, h, true);
    } else {
        viewer_texture_->draw(viewer->view_port());
    }

    *handled = true;
    modified_ = false;
}

void MiniatureViewer::viewer_buffer(BufferViewer *viewer, const uint8_t** buffer, uint32_t *w, uint32_t *h) {
    *buffer = clipping_buffer_.get();
    *w = miniature_buffer_w_;
    *h = miniature_buffer_h_;
}

void MiniatureViewer::update_preview(Clipping *clipping) {
    clipping_ = clipping;
    miniature_buffer_w_ = 0;
    miniature_buffer_h_ = 0;

    uint32_t preview_w = clipping_->w(), preview_h = clipping_->h();

    viewport_t vp(0, 0, w(), h());
    float fit_scale = vp.fit(&preview_w, &preview_h);

    unsigned int required_size = preview_w * preview_h * 3;

    if (required_size < 1) {
        clipping_buffer_.reset();
        return;
    }

    if (buffer_size_ < required_size || !clipping_buffer_) {
        buffer_size_ = required_size;
        clipping_buffer_.reset(new uint8_t[buffer_size_], [](uint8_t *b) { delete[] b;});
    }

    miniature_buffer_w_ = preview_w;
    miniature_buffer_h_ = preview_h;

    auto key = clipping_->at(clipping_->player()->info()->position());
    key.scale *= fit_scale;

    clipping_->render(key, preview_w, preview_h, clipping_buffer_.get());

    modified_ = true;
    redraw();
}

}  // namespace vcutter

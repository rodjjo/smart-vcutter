/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/viewer/miniature_viewer.h"

namespace vcutter {

MiniatureViewer::MiniatureViewer(uint32_t x, uint32_t y, uint32_t w, uint32_t h): BufferViewer(this, this, x, y, w, h) {
    clipping_ = NULL;
    modified_ = true;
    miniature_buffer_w_ = 0;
    miniature_buffer_h_ = 0;
}

MiniatureViewer::~MiniatureViewer() {
}

void MiniatureViewer::invalidate() {
    clipping_ = NULL;
    modified_ = true;
    miniature_buffer_w_ = 0;
    miniature_buffer_h_ = 0;
    render_buffer_.reset();
}

void MiniatureViewer::update_preview(Clipping *clipping) {
    clipping_ = clipping;

    if (miniature_buffer_w_ != clipping_->w() || miniature_buffer_h_ != clipping_->h() || !render_buffer_) {
        miniature_buffer_w_ = clipping_->w();
        miniature_buffer_h_ = clipping_->h();
        uint32_t required_size = clipping_->req_buffer_size();
        render_buffer_.reset(new uint8_t[required_size], [](uint8_t *b) { delete[] b;});
    }

    clipping_->render(clipping_->at(clipping_->player()->info()->position()), render_buffer_.get());
    modified_ = true;
    redraw();
}

void MiniatureViewer::viewer_draw(BufferViewer *viewer, bool *handled, const unsigned char* buffer, uint32_t w, uint32_t h) {
    if (!clipping_ || !render_buffer_ || clipping_->player()->is_playing())  {
        return;
    }

    if (modified_ || !viewer_texture_) {
        if (!viewer_texture_)  {
           viewer_texture_.reset(new ViewerTexture());
        }

        viewer_texture_->draw(viewer->view_port(), render_buffer_.get(), miniature_buffer_w_, miniature_buffer_h_, true);
    } else {
        viewer_texture_->draw(viewer->view_port());
    }

    *handled = true;
    modified_ = false;
}

void MiniatureViewer::viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) {
    if (!clipping_) {
        return;
    }

    *buffer = render_buffer_.get();
    *w = miniature_buffer_w_;
    *h = miniature_buffer_h_;
}

}  // namespace vcutter

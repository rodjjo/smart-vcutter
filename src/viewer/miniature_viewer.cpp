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
    player_ = NULL;
    miniature_buffer_w_ = 0;
    miniature_buffer_h_ = 0;
    buffer_size_ = 0;
    modified_ = true;
    clipping_buffer_.reset();
}

void MiniatureViewer::viewer_draw(BufferViewer *viewer, bool *handled, const unsigned char* buffer, uint32_t w, uint32_t h) {
    if (player_->is_playing()) {
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

void MiniatureViewer::viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) {
    *buffer = clipping_buffer_.get();
    *w = miniature_buffer_w_;
    *h = miniature_buffer_h_;
}

void MiniatureViewer::update_preview(PlayerWrapper *player, ClippingKeeper *keeper) {
    player_ = player;
    miniature_buffer_w_ = 0;
    miniature_buffer_h_ = 0;

    keeper->set_video_dimensions(player->info()->w(), player->info()->h());

    uint32_t preview_w = keeper->get_width(), preview_h = keeper->get_height();

    viewport_t vp(0, 0, w(), h());
    float fit_scale = vp.fit(&preview_w, &preview_h);

    unsigned int required_size = preview_w * preview_h * 3;

    if (required_size < 1) {
        clipping_buffer_.reset();
        return;
    }

    if (buffer_size_ < required_size || !clipping_buffer_) {
        buffer_size_ = required_size;
        clipping_buffer_.reset(new unsigned char[buffer_size_], [](unsigned char *b) { delete[] b;});
    }

    miniature_buffer_w_ = preview_w;
    miniature_buffer_h_ = preview_h;

    auto key = keeper->get_key(player->info()->position());
    key.scale *= fit_scale;

    paint_clipping(player->info()->buffer(),  player->info()->w(), player->info()->h(), key, preview_w, preview_h, clipping_buffer_.get());

    modified_ = true;
    redraw();
}

}  // namespace vcutter

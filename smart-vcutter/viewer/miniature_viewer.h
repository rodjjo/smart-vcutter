/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_VIEWER_MINIATURE_VIEWER_H
#define SMART_VCUTTER_VIEWER_MINIATURE_VIEWER_H

#include <memory>

#include "smart-vcutter/wrappers/video_player.h"
#include "smart-vcutter/clippings/clipping_keeper.h"
#include "smart-vcutter/viewer/buffer_viewer.h"
#include "smart-vcutter/viewer/viewer_texture.h"

namespace vcutter {

class MiniatureViewer: public BufferViewer, public BufferSupplier, public DrawHandler {
public:
    MiniatureViewer(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    virtual ~MiniatureViewer();
    void invalidate();
    void update_preview(PlayerWrapper *player, ClippingKeeper *keeper);

private:
    void viewer_draw(BufferViewer *viewer, bool *handled, const unsigned char* buffer, uint32_t w, uint32_t h) override;
    void viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) override;

private:
    PlayerWrapper *player_;
    std::unique_ptr<ViewerTexture> viewer_texture_;
    bool modified_;
    bool should_update_;
    uint32_t miniature_buffer_w_;
    uint32_t miniature_buffer_h_;
    uint32_t buffer_size_;
    std::shared_ptr<unsigned char> clipping_buffer_;
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_VIEWER_MINIATURE_VIEWER_H
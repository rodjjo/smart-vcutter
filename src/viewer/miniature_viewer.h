/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_VIEWER_MINIATURE_VIEWER_H_
#define SRC_VIEWER_MINIATURE_VIEWER_H_

#include <memory>

#include "src/wrappers/video_player.h"
#include "src/clippings/clipping.h"
#include "src/viewer/buffer_viewer.h"
#include "src/viewer/viewer_texture.h"

namespace vcutter {

class MiniatureViewer: public BufferViewer, public BufferSupplier, public DrawHandler {
public:
    MiniatureViewer(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    virtual ~MiniatureViewer();
    void invalidate();
    void update_preview(Clipping *clipping);

private:
    void viewer_draw(BufferViewer *viewer, bool *handled, const unsigned char* buffer, uint32_t w, uint32_t h) override;
    void viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) override;

private:
    Clipping *clipping_;
    std::unique_ptr<ViewerTexture> viewer_texture_;
    bool modified_;
    uint32_t miniature_buffer_w_;
    uint32_t miniature_buffer_h_;
    std::shared_ptr<unsigned char> render_buffer_;
};

}  // namespace vcutter

#endif  // SRC_VIEWER_MINIATURE_VIEWER_H_

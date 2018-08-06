#ifndef SMART_VCUTTER_VIEWER_CLIPPING_EDITOR_H
#define SMART_VCUTTER_VIEWER_CLIPPING_EDITOR_H

#include <memory>

#include <FL/Fl_RGB_Image.H>

#include "smart-vcutter/wrappers/video_player.h"
#include "smart-vcutter/clippings/clipping_keeper.h"
#include "smart-vcutter/viewer/buffer_viewer.h"
#include "smart-vcutter/viewer/viewer_texture.h"
#include "smart-vcutter/viewer/editor/clipping_operations.h"

#define ROTATE_OPERATION_NAME "rotate"
#define RESIZE_OPERATION_NAME "resize"
#define DRAG_OPERATION_NAME "drag"
#define MAGIC_DEFINE_OPERATION_NAME "magic-define"
#define MAGIC_USE_OPERATION_NAME "magic-use"

namespace vcutter {


class ClippingEditor: public BufferViewer, public BufferSupplier, public DrawHandler {
public:
    ClippingEditor(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    virtual ~ClippingEditor();
    void invalidate();
    void update(PlayerWrapper *player, ClippingKeeper *keeper);
    void draw_operations();
    bool first_frame_cache_initialized();
    bool last_frame_cache_initialized();
    bool key_changed(bool clear_flag);
    void activate_operation(const char *operation_name);
    void cancel_operations();
    int operation_option(const char *operation_name, const char *option_name);
    void operation_option(const char *operation_name, const char *option_name, int value);
    void toggle_compare_box();
    void wink_compare_box();
    bool compare_box();
private:
    void register_operations();
    void viewer_draw(BufferViewer *viewer, bool *handled, const unsigned char* buffer, uint32_t w, uint32_t h) override;
    void viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) override;
    void viewer_mouse_cancel(BufferViewer *viewer) override;
    void viewer_mouse_alt(BufferViewer *viewer) override;
    void viewer_mouse_down(BufferViewer *viewer, bool left_pressed, bool right_pressed, int x, int y) override;
    void viewer_mouse_move(BufferViewer *viewer, bool left_pressed, bool right_pressed, int dx, int dy, int mx, int my) override;
    void viewer_mouse_up(BufferViewer *viewer, bool left_pressed, bool right_pressed, int dx, int dy, int ux, int uy) override;
    void viewer_after_draw(BufferViewer *viewer) override;
    void draw_compare_box();
    Fl_RGB_Image *current_cursor();
    void define_cursor();
    void check_key_count();
private:
    bool initialized_caches_[3];
    int frame_numbers_[3];
    int prev_key_count_;
    bool modified_;
    bool should_update_;
    bool compare_box_;
    bool compare_box_wink_;
    Fl_RGB_Image *last_cursor_;
    PlayerWrapper *player_;
    ClippingKeeper *keeper_;

    ClippingOperationSet operation_set_;

    std::unique_ptr<ViewerTexture> text_first_frame_;
    std::unique_ptr<ViewerTexture> text_curr_frame_;
    std::unique_ptr<ViewerTexture> text_last_frame_;

};

}  // namespace vcutter

#endif  // SMART_VCUTTER_VIEWER_CLIPPING_EDITOR_H
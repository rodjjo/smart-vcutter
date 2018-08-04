#ifndef SMART_VCUTTER_VIEWER_RESIZE_OPERATION_H
#define SMART_VCUTTER_VIEWER_RESIZE_OPERATION_H

#include "smart-vcutter/viewer/editor/clipping_operations.h"
#include "smart-vcutter/viewer/viewer_texture.h"

namespace vcutter {

class ResizeOperation: public ClippingOperation {
 public:
    explicit ResizeOperation(const char *name);
    virtual ~ResizeOperation();
    bool active() override;
    void draw_dragging_points() override;
    clipping_key_t get_transformed_key() override;
    void cancel() override;
    bool should_redraw() override;
    void mouse_changed(char direction) override;
    Fl_RGB_Image *current_cursor() override;
 private:
     void set_mouse_distance(int distance);
 private:
    bool active_;
    bool should_redraw_;
    int mouse_distance_;
    std::shared_ptr<Fl_RGB_Image> cursor_;
    std::shared_ptr<ViewerTexture> resize_point_;
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_VIEWER_RESIZE_OPERATION_H
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_VIEWER_EDITOR_ROTATE_OPERATION_H_
#define SRC_VIEWER_EDITOR_ROTATE_OPERATION_H_

#include "src/viewer/editor/clipping_operations.h"
#include "src/viewer/viewer_texture.h"

namespace vcutter {

class RotateOperation: public ClippingOperation {
 public:
    explicit RotateOperation(const char *name);
    virtual ~RotateOperation();
    bool active() override;
    void draw_dragging_points() override;
    ClippingKey get_transformed_key() override;
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
    std::shared_ptr<ViewerTexture> rotate_point_;
};

}  // namespace vcutter

#endif  // SRC_VIEWER_EDITOR_ROTATE_OPERATION_H_

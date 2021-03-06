/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_VIEWER_EDITOR_DRAG_OPERATION_H_
#define SRC_VIEWER_EDITOR_DRAG_OPERATION_H_

#include "src/viewer/editor/clipping_operations.h"

namespace vcutter {

class DragOperation: public ClippingOperation {
 public:
    explicit DragOperation(const char* name);
    virtual ~DragOperation();
    bool active() override;
    void draw_dragging_points() override;
    ClippingKey get_transformed_key() override;
    void cancel() override;
    void alt_pressed() override;
    bool should_redraw() override;
    void mouse_changed(char direction) override;
    Fl_RGB_Image *current_cursor() override;
 private:
    bool active_;
    bool in_box_;
    bool mouse_down_;
    bool should_redraw_;
    std::shared_ptr<Fl_RGB_Image> cursor_;
};

}  // namespace vcutter

#endif  // SRC_VIEWER_EDITOR_DRAG_OPERATION_H_

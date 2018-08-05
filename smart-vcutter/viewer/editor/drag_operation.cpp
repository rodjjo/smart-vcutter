#include <FL/Fl.H>
#include <GL/gl.h>

#include "smart-vcutter/common/calcs.h"
#include "smart-vcutter/data/xpm.h"

#include "smart-vcutter/viewer/editor/drag_operation.h"

namespace vcutter {

DragOperation::DragOperation(const char *name) : ClippingOperation(name) {
    active_ = false;
    should_redraw_ = false;
    mouse_down_ = false;
    in_box_ = false;
    cursor_ = xpm::image(xpm::cursor_drag);
}

DragOperation::~DragOperation() {
};

bool DragOperation::active() {
    return active_;
}

void DragOperation::draw_dragging_points() {
    should_redraw_ = false;
}

clipping_key_t DragOperation::get_transformed_key() {
    auto key = keeper()->get_key(player()->info()->position());
    float nx = key.px; float ny = key.py;

    if (active_) {
        if (!mouse_down_) {
            nx = mouse_move_x();
            ny = mouse_move_y();
            buffer_coords(view_port(), player()->info()->w(), player()->info()->h(), &nx, &ny);
        } else {
            float dx = mouse_down_x(), dy = mouse_down_y();
            nx = mouse_move_x();
            ny = mouse_move_y();
            buffer_coords(view_port(), player()->info()->w(), player()->info()->h(), &nx, &ny);
            buffer_coords(view_port(), player()->info()->w(), player()->info()->h(), &dx, &dy);
            nx -= dx;
            ny -= dy;
            nx += key.px;
            ny += key.py;
        }
    }

    if (Fl::event_shift()) {
        key.py = ny;
    } else if (Fl::event_command() || Fl::event_ctrl()) {
        key.px = nx;
    } else {
        key.px = nx;
        key.py = ny;
    }

    return key;
};

bool DragOperation::should_redraw() {
    return should_redraw_;
}

void DragOperation::mouse_changed(char direction) {
    auto b = current_clipping_box(view_port(), player(), keeper());
    bool in_box = b.contours_point(mouse_move_x(), mouse_move_y());

    if (direction == 0) {
        if (active_) {
            should_redraw_ = true;
        } else if (!should_redraw_) {
            if (in_box_ != in_box) {
                in_box_ = in_box;
                should_redraw_ = true;
            }
        }

        if (Fl::event_alt()) {
            alt_pressed();
        }

        return;
    } else if (direction < 0) {
        if (!in_box) {
            return;
        }

        if (!active_) {
            active_ = true;
            mouse_down_ = true;
            return;
        }
    }

    add_key();

    active_ = false;
    should_redraw_ = true;
    mouse_down_ = false;
}

void DragOperation::alt_pressed() {
    active_ = true;
    mouse_down_ = false;
    should_redraw_ = true;
}

void DragOperation::cancel() {
    active_ = false;
    mouse_down_ = false;
}

Fl_RGB_Image *DragOperation::current_cursor() {
    if (active_ || current_clipping_box(view_port(), player(), keeper()).contours_point(mouse_move_x(), mouse_move_y())) {
        return cursor_.get();
    }

    return NULL;
}

}  // namespace vcutter

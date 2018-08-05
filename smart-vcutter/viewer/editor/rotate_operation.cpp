#include <FL/Fl.H>
#include <GL/gl.h>

#include "smart-vcutter/common/calcs.h"
#include "smart-vcutter/data/xpm.h"

#include "smart-vcutter/viewer/editor/rotate_operation.h"

namespace vcutter {

RotateOperation::RotateOperation(const char *name) : ClippingOperation(name) {
    active_ = false;
    should_redraw_ = false;
    cursor_ = xpm::image(xpm::cursor_rotate);
    auto img = xpm::image(xpm::editor_rotate, 0);
    rotate_point_.reset(new ViewerTexture(reinterpret_cast<const uint8_t*>(img->data()[0]), img->w(), img->h(), false, true));
    mouse_distance_ = 100;
}

RotateOperation::~RotateOperation() {
};

bool RotateOperation::active() {
    return active_;
}

void RotateOperation::draw_dragging_points() {
    should_redraw_ = false;
    if (active_) {
        return;
    }
    
    auto b = current_clipping_box(view_port(), player(), keeper());
    bool should_draw_points = b.contours_point(mouse_move_x(), mouse_move_y());
    
    int distance = mouse_distance_;

    if (!should_draw_points) {
        should_draw_points = distance < 3;
    } else {
        distance = 0;
    }

    if (should_draw_points) {
        for (char i = 0; i < 4; ++i) {
            rotate_point_->draw(view_port(),  b[i].x, b[i].y, 1.0);
        }
    }
}

clipping_key_t RotateOperation::get_transformed_key() {
    auto key = keeper()->get_key(player()->info()->position());

    float mx = mouse_move_x();
    float my = mouse_move_y();
    float dx = mouse_down_x();
    float dy = mouse_down_y();

    buffer_coords(view_port(), player()->info()->w(), player()->info()->h(), &mx, &my);
    buffer_coords(view_port(), player()->info()->w(), player()->info()->h(), &dx, &dy);

    float mdown_angle = point_t(dx - key.px, dy - key.py).angle_0_360();
    float mmove_angle = point_t(mx - key.px, my - key.py).angle_0_360();
    float angle = (static_cast<int>((key.angle +  mmove_angle - mdown_angle) * 1000) % 360000) / 1000.0f;
    while (angle < 0) {
        angle += 360;
    }
    key.angle = angle;


    return key;
};

bool RotateOperation::should_redraw() {
    return should_redraw_;
}

void RotateOperation::mouse_changed(char direction) {
    if (direction > 0 && !active_) {
        return;
    }
    
    auto b = current_clipping_box(view_port(), player(), keeper());
    float mouse_dist = 10000;
    float d;

    for (char c = 0; c < 4; ++c) {
        d = point_t(mouse_move_x(), mouse_move_y()).distance_to(b[c].x, b[c].y);
        if (d < mouse_dist) {
            mouse_dist = d;
        }
    }

    if (!active_) {
        if (mouse_dist < 16) {
            set_mouse_distance(0);
        } else if (mouse_dist < 32) {
            set_mouse_distance(1);
        } else {
            set_mouse_distance(100);
        }
    }

    if (direction < 0 && mouse_distance_ == 0) {
        active_ = true;
    }

    if (direction > 0 && active_) {
        add_key();
        active_ = false;
        should_redraw_ = true;
    }

    should_redraw_ = should_redraw_ || active_;
}

void RotateOperation::set_mouse_distance(int distance) {
    if (distance == mouse_distance_) {
        return;
    }
    should_redraw_ = true;
    mouse_distance_ = distance;
}

void RotateOperation::cancel() {
    active_ = false;
}

Fl_RGB_Image *RotateOperation::current_cursor() {
    if (active_ || mouse_distance_ == 0) {
        return cursor_.get();
    }
    return NULL;
}

}  // namespace vcutter

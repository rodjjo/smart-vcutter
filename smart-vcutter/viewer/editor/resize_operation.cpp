#include <FL/Fl.H>
#include <GL/gl.h>

#include "smart-vcutter/common/calcs.h"
#include "smart-vcutter/data/xpm.h"

#include "smart-vcutter/viewer/editor/resize_operation.h"

namespace vcutter {

ResizeOperation::ResizeOperation(const char *name) : ClippingOperation(name) {
    active_ = false;
    should_redraw_ = false;
    cursor_ = xpm::image(xpm::cursor_resize);
    auto img = xpm::image(xpm::editor_resize, 0);
    resize_point_.reset(new ViewerTexture(reinterpret_cast<const uint8_t*>(img->data()[0]), img->w(), img->h(), false, true));
    mouse_distance_ = 100;
}

ResizeOperation::~ResizeOperation() {
};

bool ResizeOperation::active() {
    return active_;
}

void ResizeOperation::draw_dragging_points() {
    should_redraw_ = false;
    if (active_) {
        return;
    }
    
    auto b = current_clipping_box(view_port(), player(), keeper());
    bool should_draw_points = mouse_in_box(b, mouse_move_x(), mouse_move_y());
    
    int distance = mouse_distance_;

    if (!should_draw_points) {
        should_draw_points = distance < 3;
    } else {
        distance = 0;
    }

    if (should_draw_points) {
        int w;
        for (char i = 0; i < 4; ++i) {
            w = (i + 1) % 4;
            resize_point_->draw(view_port(), (b[w].x + b[i].x) / 2, (b[w].y + b[i].y) / 2, 1.0);
        }
    }
}

clipping_key_t ResizeOperation::get_transformed_key() {
    auto key = keeper()->get_key(player()->info()->position());

    float mx = mouse_move_x();
    float my = mouse_move_y();
    float dx = mouse_down_x();
    float dy = mouse_down_y();

    buffer_coords(view_port(), player()->info()->w(), player()->info()->h(), &mx, &my);
    buffer_coords(view_port(), player()->info()->w(), player()->info()->h(), &dx, &dy);

    float dw, dh;
    dw = mx - key.px;
    dh = my - key.py;
    float dista1 = sqrt(dw * dw + dh * dh);

    dw = dx - key.px;
    dh = dy - key.py;
    float dista2 = sqrt(dw * dw + dh * dh);

    if (dista1 > 0 && dista2 > 0) {
        key.scale *= ((1.0f / dista2) * dista1);
    }

    return key;
};

bool ResizeOperation::should_redraw() {
    return should_redraw_;
}

void ResizeOperation::mouse_changed(char direction) {
    if (direction > 0 && !active_) {
        return;
    }

    auto box = current_clipping_box(view_port(), player(), keeper());
    box_t b;
    int w;

    for (int i = 0; i < 4; ++i) {
        w = (i + 1) % 4;
        b[i].x = (box[w].x + box[i].x) / 2;
        b[i].y = (box[w].y + box[i].y) / 2;
    }

    float mouse_dist = 10000;
    float d;
    for (char c = 0; c < 4; ++c) {
        d = get_dista(mouse_move_x(), mouse_move_y(), b[c].x, b[c].y);
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

void ResizeOperation::cancel() {
    active_ = false;
}

Fl_RGB_Image *ResizeOperation::current_cursor() {
    if (active_ || mouse_distance_ == 0) {
        return cursor_.get();
    }
    return NULL;
}

void ResizeOperation::set_mouse_distance(int distance) {
    if (distance == mouse_distance_) {
        return;
    }
    should_redraw_ = true;
    mouse_distance_ = distance;
}

}  // namespace vcutter

#include <FL/Fl.H>
#include <GL/gl.h>

#include "smart-vcutter/common/calcs.h"
#include "smart-vcutter/data/xpm.h"

#include "smart-vcutter/viewer/editor/magic_operation.h"

namespace vcutter {

 MagicOperation::MagicOperation(const char* name, bool use_reference): ClippingOperation(name) {
    active_ = false;
    should_redraw_ = true;
    use_reference_ =  use_reference;
    preview_line_ = false;
    points_defined_ = false;
    drag_point1_ = false;
    drag_point2_ = false;
    opt_x_enabled_ = true;
    opt_y_enabled_ = true;
    opt_scale_enabled_ = true;
    opt_rotate_enabled_ = true;
    frame_ = -1;
    px1_ = 0;
    py1_ = 0;
    px2_ = 0;
    py2_ = 0;
    near_points_ = false;
    near_apply_ = false;
    cursor_ = xpm::image(xpm::cursor_dot);
    auto img = xpm::image(xpm::editor_apply);
    apply_.reset(new ViewerTexture(reinterpret_cast<const uint8_t*>(img->data()[0]), img->w(), img->h(), false, true));
    img = xpm::image(xpm::editor_apply_off);
    apply_off_.reset(new ViewerTexture(reinterpret_cast<const uint8_t*>(img->data()[0]), img->w(), img->h(), false, true));
    img = xpm::image(xpm::editor_target1);
    target1_.reset(new ViewerTexture(reinterpret_cast<const uint8_t*>(img->data()[0]), img->w(), img->h(), false, true));;
    img = xpm::image(xpm::editor_target2);
    target2_.reset(new ViewerTexture(reinterpret_cast<const uint8_t*>(img->data()[0]), img->w(), img->h(), false, true));;
 }

MagicOperation::~MagicOperation() {
}

void MagicOperation::activate() {
    if (!player() || player()->is_playing()) {
        cancel();
        return;
    }
    cancel();
    active_ = true;
    frame_ = player()->info()->position(); 
}

bool MagicOperation::active() {
    return active_;
}

void MagicOperation::to_gl_coords(float *x, float *y) {
    const int *vp = view_port();
    *x = (*x) * (2.0f / vp[2]) - 1.0f;
    *y = (vp[3] - (*y)) * (2.0f / vp[3]) - 1.0f;
}

void MagicOperation::draw() {
    should_redraw_ = false;

    if (!active_) {
        return;
    }

    if (preview_line_) {
        glColor3f(1.0, 0, 0);
        float x1 = mouse_down_x(), y1 = mouse_down_y(), x2 = mouse_move_x(), y2 = mouse_move_y();
        to_gl_coords(&x1, &y1);
        to_gl_coords(&x2, &y2);
        
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    if (points_defined_) {
        glColor3f(0.0, 0, 1.0);
        float x1 = px1_, y1 = py1_, x2 = px2_, y2 = py2_;
        to_gl_coords(&x1, &y1);
        to_gl_coords(&x2, &y2);
        
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();

        if (near_points_) {
            if (!drag_point1_) {
                target1_->draw(view_port(), px1_, py1_, 1.0);
            }
            if (!drag_point2_) {
                target2_->draw(view_port(), px2_, py2_, 1.0);
            }
        }
        if (near_apply_) {
            apply_->draw(view_port(), (px2_ + px1_) / 2.0, (py2_ + py1_) / 2.0, 1.0);
        } else {
            apply_off_->draw(view_port(), (px2_ + px1_) / 2.0, (py2_ + py1_) / 2.0, 1.0);
        }
    }
}

clipping_key_t MagicOperation::get_transformed_key() {
    auto key = keeper()->get_key(player()->info()->position());
    if (!use_reference_ || (!preview_line_ && !points_defined_)) {
        return key;
    }

    int ref_frame;
    float px1, py1, px2, py2, rx1, ry1, rx2, ry2;
    if (!keeper()->get_reference(&ref_frame, &rx1, &ry1, &rx2, &ry2)) {
        return key;
    }
    
    key = keeper()->get_key(ref_frame);
    key.frame = player()->info()->position();

    if (points_defined_) {
        px1 = px1_;
        px2 = px2_;
        py1 = py1_;
        py2 = py2_;
    } else {
        px1 = mouse_down_x();
        py1 = mouse_down_y();
        px2 = mouse_move_x();
        py2 = mouse_move_y();
    }

    return magic_tool(
        key, 
        keeper()->get_width(), keeper()->get_height(),
        player()->info()->w(), player()->info()->h(),
        rx1, ry1, rx2, ry2, 
        px1, py1, px2, py2, 
        opt_rotate_enabled_, opt_scale_enabled_, opt_x_enabled_, opt_y_enabled_);
};

void MagicOperation::cancel() {
    active_ = false;
    points_defined_ = false;
    preview_line_ = false;
    near_points_ = false;
    near_apply_ = false;
    should_redraw_ = true;
    frame_ = -1;
}

bool MagicOperation::should_redraw() {
    return should_redraw_;
};

void MagicOperation::mouse_changed(char direction) {
    if (active_) {
        if (frame_ != player()->info()->position()) { // cancel operation if frame was changed
            cancel();
            return;
        }
    }
    
    if (!points_defined_) {
        if (direction > 0) {
            points_defined_ = true;
            should_redraw_ = true;
            px1_ = mouse_down_x();
            py1_ = mouse_down_y();
            px2_ = mouse_move_x();
            py2_ = mouse_move_y();
            preview_line_ = false;
        }
        if (direction < 0) {
            preview_line_ = true;
            should_redraw_ = true;
        }
        return;
    } else {
        set_near_points(near_point_1() || near_point_2());
        set_near_apply(near_apply());
        if (direction < 0) {
            drag_point1_ = drag_point1_ || near_point_1();
            drag_point2_ = !drag_point1_ && (drag_point2_ || near_point_2());
        }
        if (drag_point1_) {
            px1_ = mouse_move_x();
            py1_ = mouse_move_y(); 
            should_redraw_ = true;
        } else if (drag_point2_) {
            px2_ = mouse_move_x();
            py2_ = mouse_move_y();
            should_redraw_ = true;
        }
    }

    if (direction > 0) {
        if (points_defined_ && !drag_point1_ && !drag_point2_) {
            if (near_apply_) {
                apply();
                return;
            } else {
                cancel();
                return;
            }
        }
        drag_point1_ = false;
        drag_point2_ = false;
    }

    if (direction == 0) {
        should_redraw_ = should_redraw_ || preview_line_;
    }
};

bool MagicOperation::near_point_1() {
    return get_dista(mouse_move_x(), mouse_move_y(), px1_, py1_) < 16;
}

bool MagicOperation::near_point_2() {
    return get_dista(mouse_move_x(), mouse_move_y(), px2_, py2_) < 16;
}

bool MagicOperation::near_apply() {
    return get_dista(
        mouse_move_x() , 
        mouse_move_y(), 
        (px2_ + px1_) / 2.0, 
        (py2_ + py1_) / 2.0) < 16;
}

Fl_RGB_Image *MagicOperation::current_cursor() {
    if (active_) {
        return cursor_.get();
    }
    return NULL;
}

void MagicOperation::set_near_points(bool value) {
    if (value == near_points_) {
        return;
    }
    should_redraw_ = true;
    near_points_ = value;
}

void MagicOperation::set_near_apply(bool value) {
    if (value == near_apply_) {
        return;
    }
    should_redraw_ = true;
    near_apply_ = value;
}

void MagicOperation::set_option(const char *opt_name, int value) {
    bool bool_value = value != 0;
    if (strcmp("x_enabled", opt_name) == 0) {
        opt_x_enabled_ = bool_value;
    } else if (strcmp("y_enabled", opt_name) == 0) {
        opt_y_enabled_ = bool_value;
    } else if (strcmp("scale_enabled", opt_name) == 0) {
        opt_scale_enabled_ = bool_value;
    } else if (strcmp("rotate_enabled", opt_name) == 0) {
        opt_rotate_enabled_ = bool_value;
    }
}

int MagicOperation::get_option(const char *opt_name) {
    if (strcmp("x_enabled", opt_name) == 0) {
        return opt_x_enabled_;
    } else if (strcmp("y_enabled", opt_name) == 0) {
        return opt_y_enabled_;
    } else if (strcmp("scale_enabled", opt_name) == 0) {
        return opt_scale_enabled_;
    } else if (strcmp("rotate_enabled", opt_name) == 0) {
        return opt_rotate_enabled_;
    }
    return 0;
};

void MagicOperation::apply() {
    if (!active_ || !points_defined_) {
        cancel();
        return;
    }

    if (use_reference_) {
        add_key();
    } else {
        keeper()->set_reference(player()->info()->position(), px1_, py1_, px2_, py2_);
        modify();
    }

    cancel();
}

}  // namespace vcutter

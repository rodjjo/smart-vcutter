#include <FL/Fl.H>
#include <GL/gl.h>

#include "smart-vcutter/viewer/editor/clipping_operations.h"

namespace vcutter {


ClippingOperation::ClippingOperation(const char *name) {
    name_ = name;
    player_ = NULL;
    keeper_ = NULL;
    modified_ = NULL;
    mouse_move_x_  = 0;
    mouse_move_y_  = 0;
    mouse_down_x_  = 0;
    mouse_down_y_  = 0;
    view_port_[0] = 0;
    view_port_[1] = 0;
    view_port_[2] = 1;
    view_port_[3] = 1;
}

void ClippingOperation::view_port(const int *vp) {
    view_port_[0] = vp[0];
    view_port_[1] = vp[1];
    view_port_[2] = vp[2];
    view_port_[3] = vp[3];
}

const int *ClippingOperation::view_port() {
    return view_port_;
}

const char *ClippingOperation::name() {
    return name_.c_str();
}

void ClippingOperation::draw() {
}

void ClippingOperation::draw_dragging_points() {
}

void ClippingOperation::set_option(const char *opt_name, int value) {
};

void ClippingOperation::alt_pressed() {
}

 int ClippingOperation::get_option(const char *opt_name) { 
    return 0;
}

void ClippingOperation::register_operation(PlayerWrapper **player_var, ClippingKeeper **keeper_var, bool *modified_var) {
    player_ = player_var;
    keeper_ = keeper_var;
    modified_ = modified_var;
}

void ClippingOperation::mouse_down(bool left_pressed, bool right_pressed, int x, int y) {
    mouse_move_x_  = x;
    mouse_move_y_  = y;
    mouse_down_x_  = x;
    mouse_down_y_  = y;

    if (left_pressed) {
        mouse_changed(-1);
    }
}

void ClippingOperation::mouse_move(bool left_pressed, bool right_pressed, int dx, int dy, int mx, int my) {
    mouse_move_x_  = mx;
    mouse_move_y_  = my;
    mouse_down_x_  = dx;
    mouse_down_y_  = dy;

    mouse_changed(0);
}

void ClippingOperation::mouse_up(bool left_pressed, bool right_pressed, int dx, int dy, int ux, int uy) {
    mouse_move_x_  = ux;
    mouse_move_y_  = uy;
    mouse_down_x_  = dx;
    mouse_down_y_  = dy;

    if (left_pressed) {
        mouse_changed(1);
    }
}

int ClippingOperation::mouse_down_x() {
    return mouse_down_x_;
}

int ClippingOperation::mouse_down_y() {
    return mouse_down_y_;
}

int ClippingOperation::mouse_move_x() {
    return mouse_move_x_;
}

int ClippingOperation::mouse_move_y() {
    return mouse_move_y_;
}

void ClippingOperation::_cancel() {
    cancel();
}

PlayerWrapper *ClippingOperation::player() {
    return *player_;
}

ClippingKeeper *ClippingOperation::keeper() {
    return *keeper_;
}

void ClippingOperation::add_key() {
    player()->pause();
    keeper()->add_key(get_transformed_key());
    modify();
}

void ClippingOperation::modify() {
    *modified_ = true;
}

ClippingOperationSet::ClippingOperationSet(PlayerWrapper **player_var, ClippingKeeper **keeper_var) {
    player_ = player_var;
    keeper_ = keeper_var;
    modified_ = false;
}

ClippingOperationSet::~ClippingOperationSet() {

}

void ClippingOperationSet::register_operation(std::shared_ptr<ClippingOperation> operation) {
    for (auto op : operations_) {
        if (op == operation) {
            return;
        }
    }

    operations_.push_back(operation);
    operation->register_operation(player_, keeper_, &modified_);
}

clipping_key_t ClippingOperationSet::get_transformed_key() {
    clipping_key_t k;
    memset(&k, 0, sizeof(k));
    if (!*player_ || !*keeper_) {
        k.frame = 1;
        k.scale = 1;
        k.px = 2;
        k.py = 2;
        return k;
    }

    for (auto op : operations_) {
        if (op->active()) {
            return op->get_transformed_key();
        }
    }

    return (*keeper_)->get_key((*player_)->info()->position());
}

void ClippingOperationSet::draw(const int *vp) {
    if (!*player_ || !*keeper_) {
        return;
    }

    bool computed = false;
    auto b = current_clipping_box(vp, *player_, *keeper_, &computed);

    ClippingOperation *operation = NULL;
    
    for (auto op : operations_) {
        if (op->active()) {
            operation = op.get();
            break;
        }
    }

    float color[4] = { 1.0, 1.0, 1.0, 1.0};

    if (operation) {
        color[3] = 0.3;
    } else if (!computed) {
        color[3] = 0.6;
    }

    draw_box(vp, b, false, color[0], color[1], color[2], color[3]);
    
    float cw = (*keeper_)->get_width(), ch = (*keeper_)->get_height();
    float vw = (*player_)->info()->w(), vh = (*player_)->info()->h();

    if (operation) {
        operation->view_port(vp);
        operation->draw();

        b = clipping_box(adjust_bounds(operation->get_transformed_key(), cw, ch, vw, vh), cw, ch);
    
        for (char i = 0; i < 4; ++i) {
            screen_coords(vp, vw, vh, &b.p[i].x, &b.p[i].y);
        }

        color[0] = 0.0;
        color[1] = 0.0;
        color[2] = 1.0;
        color[3] = 1.0;
        draw_box(vp, b, false, color[0], color[1], color[2], color[3]);
    }
}

void ClippingOperationSet::set_option(const char *operation_name, const char *option_name, int value) {
    for (auto op : operations_) {
        if (strcmp(op->name(), operation_name) == 0) {
            op->set_option(option_name, value);
            return;
        }
    }
}

int ClippingOperationSet::get_option(const char *operation_name, const char *option_name) {
    for (auto op : operations_) {
        if (strcmp(op->name(), operation_name) == 0) {
            return op->get_option(option_name);
        }
    }
    return 0;
}

void ClippingOperationSet::draw_dragging_points(const int *vp) {
    if (!*player_ || !*keeper_) {
        return;
    }

    if (has_active_operation()) {
        return;
    }

    for (auto op : operations_) {
        op->view_port(vp);
        op->draw_dragging_points();
    }
}

bool ClippingOperationSet::modified(bool clear_flag) {
    if (modified_) {
        modified_ = !clear_flag;
        return true;
    }
    return false;
}

void ClippingOperationSet::mouse_down(const int *vp, bool left_pressed, bool right_pressed, int x, int y) {
    if (!*player_ || !*keeper_) {
        return;
    }

    for (auto op : operations_) {
        if (op->active()) {
            op->view_port(vp);
            op->mouse_down(left_pressed, right_pressed, x, y);
            return;
        }
    }

    for (auto op : operations_) {
        op->view_port(vp);
        op->mouse_down(left_pressed, right_pressed, x, y);
        if (op->active()) {
            return;
        }
    }
}

void ClippingOperationSet::mouse_move(const int *vp, bool left_pressed, bool right_pressed, int dx, int dy, int mx, int my) {
    if (!*player_ || !*keeper_) {
        return;
    }

    for (auto op : operations_) {
        if (op->active()) {
            op->view_port(vp);
            op->mouse_move(left_pressed, right_pressed, dx, dy, mx, my);
            return;
        }
    }

    for (auto op : operations_) {
        op->view_port(vp);
        op->mouse_move(left_pressed, right_pressed, dx, dy, mx, my);
        if (op->active()) {
            return;
        }
    }
}

void ClippingOperationSet::mouse_up(const int *vp, bool left_pressed, bool right_pressed, int dx, int dy, int ux, int uy) {
    if (!*player_ || !*keeper_) {
        return;
    }

    for (auto op : operations_) {
        op->view_port(vp);
        if (op->active()) {
            op->mouse_up(left_pressed, right_pressed, dx, dy, ux, uy);
            return;
        }
    }

    for (auto op : operations_) {
        op->view_port(vp);
        op->mouse_up(left_pressed, right_pressed, dx, dy, ux, uy);
        if (op->active()) {
            return;
        }
    }
}

void ClippingOperationSet::activate(const char *name) {
    for (auto op : operations_) {
        if (strcmp(op->name(), name) == 0) {
            op->activate();
            return;
        }
    }
}

bool ClippingOperationSet::is_active(const char *operation) {
    for (auto op : operations_) {
        if (strcmp(op->name(), operation) == 0) {
            return op->active();
        }
    }
    return false;
}

void ClippingOperationSet::alt_pressed() {
    if (has_active_operation()) {
        return;
    }

    for (auto op : operations_) {
        op->alt_pressed();
        if (op->active()) {
            return;
        }
    }
}

bool ClippingOperationSet::has_active_operation() {
    for (auto op : operations_) {
        if (op->active()) {
            return true;
        }
    }
    return false;
}

void ClippingOperationSet::cancel() {
    for (auto op : operations_) {
        op->_cancel();
    }
}

Fl_RGB_Image *ClippingOperationSet::current_cursor(const int *vp) {
    Fl_RGB_Image *result = NULL;

    for (auto op : operations_) {
        op->view_port(vp);
        result = op->current_cursor();
        if (result && op->active()) {
            return result;
        }
    }

    for (auto op : operations_) {
        result = op->current_cursor();
        if (result) {
            break;
        }
    }
    return result;
}

bool ClippingOperationSet::should_redraw(const int *vp) {
    if (!*player_ || !*keeper_ || (*player_)->is_playing()) {
        return false;
    }

    for (auto op : operations_) {
        op->view_port(vp);
        if (op->should_redraw()) {
            return true;
        }
    } 

    return false;
}


box_t current_clipping_box(const int *vp, PlayerWrapper *player, ClippingKeeper *keeper, bool *computed) {
    auto vw = player->info()->w(), vh = player->info()->h();
    auto cw = keeper->get_width(), ch = keeper->get_height();

    auto key = keeper->get_key(player->info()->position(), computed);

    auto b = clipping_box(adjust_bounds(key, cw, ch, vw, vh), cw, ch);
    
    for (char i = 0; i < 4; ++i) {
        screen_coords(vp, player->info()->w(), player->info()->h(), &b.p[i].x, &b.p[i].y);
    }

    return b;
}

void ClippingOperationSet::draw_box(const int *vp, box_t box, bool invert, float r, float g, float b, float a) {
    for (char i = 0; i < 4; ++i) {
        box.p[i].x = box.p[i].x * (2.0f / vp[2]) - 1.0f;
        box.p[i].y = (vp[3] - box.p[i].y) * (2.0f / vp[3]) - 1.0f;
    }

    if (invert) {
        glLogicOp(GL_INVERT);
        glEnable(GL_COLOR_LOGIC_OP);
    } 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

    glColor4f(r, g, b, a);

    glBegin(GL_LINE_STRIP);
    int w;
    for (int i = 0; i < 5; ++i) {
        w = i % 4;
        glVertex2f(box.p[w].x, box.p[w].y);
    }
    glEnd();

    if (invert) {
        glDisable(GL_COLOR_LOGIC_OP);
    }

    glDisable(GL_BLEND);
}

void ClippingOperationSet::draw_ref_line(const int *vp) {
    if (!*player_ || !*keeper_) {
        return;
    }
    int frame = -1;
    float px1, py1, px2, py2;
    if (!(*keeper_)->get_reference(&frame, &px1, &py1, &px2, &py2) || frame != (*player_)->info()->position()) {
        return;
    }

    float sx = 2.0f / vp[2];
    float sy = 2.0f / vp[3];

    px1 = px1 * sx - 1.0f;
    py1 = (vp[3] - py1) * sy - 1.0f;
    px2 = px2 * sx - 1.0f;
    py2 = (vp[3] - py2) * sy - 1.0f;

    glLogicOp(GL_INVERT);
    glEnable(GL_COLOR_LOGIC_OP);

    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    glVertex2f(px1, py1); 
    glVertex2f(px2, py2); 
    glEnd();

   glDisable(GL_COLOR_LOGIC_OP);
}


}  // namespace vcutter

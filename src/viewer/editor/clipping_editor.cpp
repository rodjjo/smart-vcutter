/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <FL/Fl.H>
#include <GL/gl.h>

#include "src/viewer/editor/clipping_editor.h"
#include "src/viewer/editor/drag_operation.h"
#include "src/viewer/editor/resize_operation.h"
#include "src/viewer/editor/rotate_operation.h"
#include "src/viewer/editor/magic_operation.h"

namespace vcutter {

ClippingEditor::ClippingEditor(
    uint32_t x, uint32_t y, uint32_t w, uint32_t h
): BufferViewer(this, this, x, y, w, h), operation_set_(&clipping_) {
    memset(&initialized_caches_, 0, sizeof(initialized_caches_));
    memset(&frame_numbers_, 0, sizeof(frame_numbers_));
    text_first_frame_.reset(new ViewerTexture());
    text_curr_frame_.reset(new ViewerTexture());
    text_last_frame_.reset(new ViewerTexture());
    modified_ = true;
    compare_box_ = false;
    compare_box_wink_ = false;
    prev_key_count_ = 0;
    clipping_ = NULL;
    last_cursor_ = NULL;
    register_operations();
}

ClippingEditor::~ClippingEditor() {

}

void ClippingEditor::register_operations() {
    operation_set_.register_operation(std::shared_ptr<ClippingOperation>(new ResizeOperation(RESIZE_OPERATION_NAME)));
    operation_set_.register_operation(std::shared_ptr<ClippingOperation>(new DragOperation(DRAG_OPERATION_NAME)));
    operation_set_.register_operation(std::shared_ptr<ClippingOperation>(new RotateOperation(ROTATE_OPERATION_NAME)));
    operation_set_.register_operation(std::shared_ptr<ClippingOperation>(new MagicOperation(MAGIC_DEFINE_OPERATION_NAME, false)));
    operation_set_.register_operation(std::shared_ptr<ClippingOperation>(new MagicOperation(MAGIC_USE_OPERATION_NAME, true)));
}

int ClippingEditor::operation_option(const char *operation_name, const char *option_name) {
    return operation_set_.get_option(operation_name, option_name);
}

void ClippingEditor::operation_option(const char *operation_name, const char *option_name, int value) {
    operation_set_.set_option(operation_name, option_name, value);
}

void ClippingEditor::activate_operation(const char *operation_name) {
    operation_set_.cancel();
    operation_set_.activate(operation_name);
}

void ClippingEditor::invalidate() {
    memset(&initialized_caches_, 0, sizeof(initialized_caches_));
    memset(&frame_numbers_, 0, sizeof(frame_numbers_));
    clipping_ = NULL;
    prev_key_count_ = 0;
    modified_ = true;
    should_update_ =  true;
    operation_set_.cancel();
}

bool ClippingEditor::key_changed(bool clear_flag) {
    return operation_set_.modified(clear_flag);
}

void ClippingEditor::check_key_count() {
    if (!clipping_) {
        return;
    }

    if (prev_key_count_ != clipping_->keys().size()) {
        modified_ = true;
        should_update_ =  true;
        prev_key_count_ = clipping_->keys().size();
        redraw();
    }
}

void ClippingEditor::update(Clipping *clipping) {
    clipping_ = clipping;
    modified_ = true;
    should_update_ =  true;
    redraw();
}

void ClippingEditor::viewer_draw(BufferViewer *viewer, bool *handled, const unsigned char* buffer, uint32_t w, uint32_t h) {
    if (!clipping_) {
        return;
    }

    if (clipping_->player()->is_playing()) {
        return;
    }

    if (!modified_) {
        if (clipping_->player()->info()->position() == clipping_->first_frame()) {
            frame_numbers_[0] = clipping_->player()->info()->position();
            if (should_update_ || !initialized_caches_[0]) {
                text_first_frame_->draw(view_port(), buffer, w, h, true);
                initialized_caches_[0] = true;
            }

            text_first_frame_->draw(view_port());
        } else if (clipping_->player()->info()->position() == clipping_->last_frame()) {
            frame_numbers_[2] = clipping_->player()->info()->position();
            if (should_update_ || !initialized_caches_[2]) {
                text_last_frame_->draw(view_port(), buffer, w, h, true);
                initialized_caches_[0] = true;
            }
            text_last_frame_->draw(view_port());
        } else {
            frame_numbers_[1] = clipping_->player()->info()->position();
            if (should_update_ || !initialized_caches_[1]) {
                text_curr_frame_->draw(view_port(), buffer, w, h, true);
                initialized_caches_[1] = true;
            }
            text_curr_frame_->draw(view_port());
        }
        if (should_update_) {
            prev_key_count_ = clipping_->keys().size();
        }
        *handled = true;
        should_update_ = false;
        viewer_after_draw(this);
    }

    modified_ = false;
}

void ClippingEditor::draw_compare_box() {
    if (!compare_box_ || compare_box_wink_) {
        return;
    }
    int first_frame = clipping_->first_frame();
    int last_frame =  clipping_->last_frame();
    int frame = clipping_->player()->info()->position();
    if (first_frame == last_frame || (frame != last_frame && frame != first_frame) || clipping_->player()->is_playing()) {
        return;
    }

    bool magic_operations = operation_set_.is_active(MAGIC_DEFINE_OPERATION_NAME) || operation_set_.is_active(MAGIC_USE_OPERATION_NAME);
    if (magic_operations) {
        return;
    }

    ViewerTexture *texture;
    box_t texture_box;
    box_t drawing_box;

    if (frame == last_frame) {
        texture = text_first_frame_.get();
        texture_box = clipping_->at(first_frame).constrained(clipping_).clipping_box(clipping_);
    } else {
        texture = text_last_frame_.get();
        texture_box = clipping_->at(last_frame).constrained(clipping_).clipping_box(clipping_);
    }

    drawing_box = operation_set_.get_transformed_key().constrained(clipping_).clipping_box(clipping_);

    if (!texture) {
        return;
    }

    texture->draw(
        view_port(),
        clipping_->player()->info()->w(),
        clipping_->player()->info()->h(),
        texture_box,
        drawing_box,
        1.0);
}

void ClippingEditor::viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) {
    if (clipping_) {
        *buffer = clipping_->player()->info()->buffer();
        *w = clipping_->player()->info()->w();
        *h = clipping_->player()->info()->h();
    }
}

bool ClippingEditor::first_frame_cache_initialized() {
    return initialized_caches_[0];
}

bool ClippingEditor::last_frame_cache_initialized() {
    return initialized_caches_[2];
}


void ClippingEditor::viewer_mouse_cancel(BufferViewer *viewer) {
    operation_set_.cancel();
}

void ClippingEditor::cancel_operations() {
    operation_set_.cancel();
}

void ClippingEditor::viewer_mouse_alt(BufferViewer *viewer) {
    operation_set_.alt_pressed();
}

void ClippingEditor::viewer_mouse_move(BufferViewer *viewer, bool left_pressed, bool right_pressed, int dx, int dy, int mx, int my) {
    operation_set_.mouse_move(view_port(), left_pressed, right_pressed, dx, dy, mx, my);
    define_cursor();
}

void ClippingEditor::viewer_mouse_down(BufferViewer *viewer, bool left_pressed, bool right_pressed, int x, int y) {
    operation_set_.mouse_down(view_port(), left_pressed, right_pressed, x, y);
}

void ClippingEditor::viewer_mouse_up(BufferViewer *viewer, bool left_pressed, bool right_pressed, int dx, int dy, int ux, int uy) {
    operation_set_.mouse_up(view_port(), left_pressed, right_pressed, dx, dy, ux, uy);
}

void ClippingEditor::toggle_compare_box() {
    compare_box_wink_ = false;
    compare_box_ = compare_box_ == false;
    redraw();
}

bool ClippingEditor::compare_box() {
    return compare_box_;
}

void ClippingEditor::viewer_after_draw(BufferViewer *viewer) {
    const viewport_t &vp = view_port();
    draw_compare_box();
    operation_set_.draw(vp);
    operation_set_.draw_ref_line(vp);
    operation_set_.draw_dragging_points(vp);
}

Fl_RGB_Image *ClippingEditor::current_cursor() {
    if (Fl::event_x_root() < this->x_root() ||
        Fl::event_y_root() < this->y_root() ||
        this->x_root() + this->w() < Fl::event_x_root() ||
        this->y_root() + this->h() < Fl::event_y_root()
    ) {
        return NULL;
    }
    return operation_set_.current_cursor(view_port());
}

void ClippingEditor::define_cursor() {
    auto next_cursor = current_cursor();

    if (next_cursor != last_cursor_) {
        last_cursor_ = next_cursor;
        if (next_cursor) {
            this->cursor(next_cursor, 15, 15);
        } else {
            this->cursor(FL_CURSOR_DEFAULT);
        }
    }
}

void ClippingEditor::wink_compare_box() {
    if (!compare_box_) {
        compare_box_wink_ = false;
        return;
    }
    int first_frame = clipping_->first_frame();
    int last_frame =  clipping_->last_frame();
    int frame = clipping_->player()->info()->position();
    if (first_frame == last_frame || (frame != last_frame && frame != first_frame) || clipping_->player()->is_playing()) {
        compare_box_wink_ = false;
        return;
    }

    bool magic_operations = operation_set_.is_active(MAGIC_DEFINE_OPERATION_NAME) || operation_set_.is_active(MAGIC_USE_OPERATION_NAME);
    if (magic_operations) {
        return;
    }

    compare_box_wink_ = compare_box_wink_ == false;
    redraw();
}

void ClippingEditor::draw_operations() {
    define_cursor();

    if (operation_set_.should_redraw(view_port())) {
        redraw();
    }

    check_key_count();
}

}  // namespace vcutter

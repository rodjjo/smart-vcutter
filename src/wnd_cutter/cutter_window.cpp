/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cmath>
#include <GL/gl.h>

#include "src/common/utils.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_cutter/cutter_window.h"


namespace vcutter {

CutterWindow::CutterWindow(Fl_Group *parent) {
    parent_ = parent;
    int parent_x = parent->x();
    int parent_y = parent->y();
    parent->position(0, 0);

    open_failure_ = false;
    wink_comparison_ = false;
    clipping_version_ = 0;
    wink_lap_ = 0;

    clipping_actions_.reset(new ClippingActions(this));

    parent->begin();
    window_ = new Fl_Group(0, 0, 100, 100);
    parent->end();

    window_->begin();
    player_bar_.reset(new PlayerBar(clipping_actions_.get(), window_));
    components_group_ = new Fl_Group(0, 0, 100, 100);
    window_->end();

    components_group_->begin();
    components_group_->box(FL_DOWN_BOX);
    clipping_editor_  = new ClippingEditor(0, 0, 100,  100);
    side_bar_.reset(new SideBar(clipping_actions_.get(), components_group_, clipping_editor_));
    components_group_->end();

    window_->hide();

    update_title();

    parent->position(parent_x, parent_y);
}

CutterWindow::~CutterWindow() {
    clear();
}

ClippingActions *CutterWindow::clipping_actions() {
    return clipping_actions_.get();
}

Clipping* CutterWindow::clipping() {
    return clipping_actions_->clipping();
}

Player *CutterWindow::player() {
    if (clipping()) {
        return clipping()->player();
    }
    return NULL;
}

void CutterWindow::update_title() {
    if (!visible() || !clipping()) {
        parent_->window()->label("Smart Loop Creator");
        return;
    }
    char title[1024] = "";
    snprintf(title, sizeof(title), "Smart Loop Creator (%dx%d)->(%dx%d) %s",
        player()->info()->w(), player()->info()->h(),
        clipping()->w(), clipping()->h(),
        clipping()->video_path().c_str());
    parent_->window()->copy_label(title);
}

void CutterWindow::resize_controls() {
    int parent_x = parent_->x();
    int parent_y = parent_->y();

    parent_->position(0, 0);
    window_->position(0, 0);
    window_->size(parent_->w(), parent_->h());
    components_group_->position(0, 0);
    components_group_->size(window_->w(), window_->h() - 30);

    player_bar_->resize_controls();

    clipping_editor_->size(parent_->w() - SideBar::default_width() - 10,  parent_->h() - player_bar_->h() - parent_->y() - 10);
    clipping_editor_->position(5, 5);
    clipping_editor_->invalidate();

    side_bar_->resize_controls();

    parent_->position(parent_x, parent_y);

    if (visible()) {
        // update_buffers(false);
        redraw_frame();
    }
}

void CutterWindow::clear(bool clear_controls) {
    clipping_actions_->close();
    open_failure_ = false;
    wink_lap_ = 0;
    clipping_version_ = 0;
    wink_comparison_ = false;
    side_bar_->viewer()->invalidate();
    clipping_editor_->invalidate();

    if (clear_controls) {
        side_bar_->update();
        player_bar_->update();
        window_->hide();
    }

    update_title();
}

std::string CutterWindow::get_video_path() {
    if (clipping()) {
        return clipping()->video_path();
    }

    return "";
}

bool CutterWindow::modified() {

    return visible() && clipping_version_ != clipping()->version();
}

uint64_t CutterWindow::modified_version() {
    if (!modified()) {
        return 0;
    }
    return clipping()->version();
}

void CutterWindow::clear_modified() {
    clipping_version_ = clipping()->version();
}

void CutterWindow::handle_clipping_keys_changed() {
    redraw_frame(true);
}

void CutterWindow::handle_clipping_opened(bool opened) {
    if (!opened) {
        clear(true);
        return;
    }

    player()->seek_frame(clipping()->first_frame());

    player_bar_->update();
    redraw_frame();

    window_->show();
    update_title();
    clipping_version_ = clipping()->version();

    // update_buffers(false);
}

void CutterWindow::close() {
    clear(true);
}


bool CutterWindow::visible() {
    return window_->visible();
}

bool CutterWindow::clipping_actions_active() {
    return visible();
}

std::shared_ptr<ClippingRender> CutterWindow::to_clipping() {
    return clipping()->clone();
}

void CutterWindow::handle_clipping_resized() {
    redraw_frame();
    update_title();
}

void CutterWindow::action_use_ref(bool positionate_x, bool positionate_y, bool rotate, bool scale) {
    if (!visible()) {
        return;
    }
    if (player()->is_playing()) {
        show_error("It's necessary to pause the video.");
        return;
    }
    if (!clipping()->ref().has_ref()) {
        show_error("It's necessary to create references before use it.");
        return;
    }
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "x_enabled", positionate_x);
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "y_enabled", positionate_y);
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "scale_enabled", scale);
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "rotate_enabled", rotate);
    clipping_editor_->activate_operation(MAGIC_USE_OPERATION_NAME);
}

void CutterWindow::action_create_ref() {
    if (!visible()) {
        return;
    }
    if (player()->is_playing()) {
        show_error("It's necessary to pause the video.");
        return;
    }
    clipping()->ref().clear_reference();
    clipping_editor_->activate_operation(MAGIC_DEFINE_OPERATION_NAME);
}

void CutterWindow::action_clear_ref() {
    if (!visible()) {
        return;
    }
    clipping()->ref().clear_reference();
    // update_buffers(false);
}

void CutterWindow::action_goto_reference() {
    uint32_t frame = 0;
    if (clipping()->ref().get_reference_frame(&frame)) {
        player()->pause();
        player()->seek_frame(frame);
        // update_buffers(false);
    }
}

void CutterWindow::double_click(void *component) {
    if (component == side_bar_->viewer()) {
        clipping_actions()->action_properties()();
    }
}

void CutterWindow::redraw_frame(bool update_key_list) {
    if (update_key_list) {
        side_bar_->update();
        side_bar_->viewer()->update_preview(clipping());
    }

    clipping_editor_->redraw();
    side_bar_->viewer()->redraw();
    player_bar_->update();
}

void CutterWindow::handle_buffer_modified() {
    // update_buffers(false);
}

void CutterWindow::update_buffers(bool frame_changed) {
    if (player()->info()->error()) {
        return;
    }

    if (frame_changed) {
        player_bar_->update();
        redraw_frame();
        side_bar_->update_selection();
        clipping_editor_->update(clipping());
        side_bar_->viewer()->update_preview(clipping());
    } else {
        if (clipping_editor_->current_clipping() != clipping()) {
            clipping_editor_->update(clipping());
        }
        clipping_editor_->draw_operations();
        if (clipping_editor_->key_changed(true)) {
            side_bar_->update();
            side_bar_->viewer()->update_preview(clipping());
        }
    }

    if (clipping()->w() < 15 || clipping()->h() < 15) {
        clipping()->wh(player()->info()->w(), player()->info()->h());
    }

    side_bar_->update(true);
}

void CutterWindow::handle_frame_changed(Player *player) {
    update_buffers(true);
}

void CutterWindow::poll_actions() {
    if (clipping()) {
        update_buffers(false);
        if (wink_comparison_ && clipping_editor_->compare_box() && !player()->is_playing()) {
            ++wink_lap_;
            if (wink_lap_ > 11) {
                wink_lap_ = 0;
                clipping_editor_->wink_compare_box();
            }
        }
    }
}

void CutterWindow::action_toggle_compare() {
    clipping_editor_->toggle_compare_box();
    wink_comparison_ = false;
}

void CutterWindow::action_toggle_compare_wink() {
    if (!visible()) {
        return;
    }
    wink_comparison_ = wink_comparison_ == false;
}

bool CutterWindow::compare_alternate() {
    return wink_comparison_;
}

bool CutterWindow::compare_enabled() {
    return clipping_editor_->compare_box();
}

void CutterWindow::cancel_operations() {
    if (visible()) {
        clipping_editor_->cancel_operations();
    }
}


}  // namespace vcutter

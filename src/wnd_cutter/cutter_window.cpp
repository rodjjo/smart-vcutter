/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cmath>
#include <GL/gl.h>

#include "src/data/xpm.h"
#include "src/common/utils.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_cutter/cutter_window.h"


namespace vcutter {

#define CLIPPING_LIST_WIDTH 320

CutterWindow::CutterWindow(Fl_Group *parent) {
    parent_ = parent;
    open_failure_ = false;
    in_key_list_ = false;
    wink_comparison_ = false;
    clipping_version_ = 0;
    selected_clip_ = 0;
    wink_lap_ = 0;
    int parent_x = parent->x();
    int parent_y = parent->y();

    parent->position(0, 0);
    parent->begin();
    window_ = new Fl_Group(0, 0, parent->w(), parent->h());
    window_->begin();

    components_group_ = new Fl_Group(0,0, window_->w(), window_->h() - 30);
    components_group_->box(FL_DOWN_BOX);
    clipping_editor_  = new ClippingEditor(5, 5, window_->w() - CLIPPING_LIST_WIDTH - 10,  window_->h() - 45);

    btn_new_key_ = new Fl_Button(clipping_editor_->w() + 7, 3, 25, 25, "");
    btn_del_key_ = new Fl_Button(btn_new_key_->x() + 27, 3, 25, 25, "");
    btn_play_interval_ = new Fl_Button(btn_del_key_->x() + 27, 3, 25, 25, "");

    key_list_ = new Fl_Select_Browser(btn_new_key_->x(), 30, CLIPPING_LIST_WIDTH, window_->h() * 30);

    viewer_ = new MiniatureViewer(btn_new_key_->x(), key_list_->y() + key_list_->h(), key_list_->w(), key_list_->w());

    components_group_->end();

    clipping_actions_.reset(new ClippingActions(this));


    player_bar_.reset(new PlayerBar(clipping_actions_.get(), window_));

    btn_new_key_->callback(button_callback, this);
    btn_del_key_->callback(button_callback, this);
    btn_play_interval_->callback(button_callback, this);

    btn_new_key_->clear_visible_focus();
    btn_del_key_->clear_visible_focus();
    btn_play_interval_->clear_visible_focus();

    btn_new_key_->tooltip("[Insert] Insert a mark at current frame.");
    btn_del_key_->tooltip("[Delete] Remove the current frame mark.");
    btn_play_interval_->tooltip("Play the animation interval.");

    window_->end();
    parent->end();

    key_list_->callback(video_list_callback, this);

    viewer_->cursor(FL_CURSOR_HAND);

    btn_del_key_->shortcut(FL_Delete);
    btn_new_key_->shortcut(FL_Insert);

    parent->position(parent_x, parent_y);

    window_->hide();

    load_images();
    update_title();
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

PlayerWrapper *CutterWindow::player() {
    if (clipping()) {
        return clipping()->player();
    }
    return NULL;
}

void CutterWindow::set_widget_image(Fl_Widget* widget, std::shared_ptr<Fl_Image> image) {
    images_.insert(image);
    widget->image(image.get());
    widget->align(FL_ALIGN_IMAGE_BACKDROP);
}

void CutterWindow::load_images() {
    set_widget_image(btn_new_key_, xpm::image(xpm::button_add));
    set_widget_image(btn_del_key_, xpm::image(xpm::button_delete));
    set_widget_image(btn_play_interval_, xpm::image(xpm::button_play));
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

    player_bar_->resize_controls();

    clipping_editor_->size(parent_->w() - CLIPPING_LIST_WIDTH - 10,  parent_->h() - player_bar_->h() - parent_->y() - 10);
    clipping_editor_->position(5, 5);
    clipping_editor_->invalidate();

    btn_new_key_->position(parent_->w() - CLIPPING_LIST_WIDTH, 3);
    btn_del_key_->position(btn_new_key_->x() + 27, 3);
    btn_play_interval_->position(btn_del_key_->x() + 27, 3);

    static_cast<Fl_Widget *>(key_list_)->position(btn_new_key_->x(), 33);
    int key_list_w = parent_->w() - btn_new_key_->x();
    key_list_->size(key_list_w, clipping_editor_->h() - key_list_w - player_bar_->h() - 38);

    viewer_->position(btn_new_key_->x(), key_list_->y() + key_list_->h());
    viewer_->size(key_list_->w(), key_list_->w());

    btn_new_key_->size(25, 25);
    btn_del_key_->size(25, 25);
    btn_play_interval_->size(25, 25);

    parent_->position(parent_x, parent_y);

    if (visible()) {
        update_buffers(true);
        redraw_frame();
    }
}

void CutterWindow::clear(bool clear_controls) {
    clipping_actions_->close();
    open_failure_ = false;
    wink_lap_ = 0;
    clipping_version_ = 0;
    wink_comparison_ = false;
    viewer_->invalidate();
    clipping_editor_->invalidate();
    if (clear_controls) {
        key_list_->clear();
        player_bar_->update();
        window_->hide();
        player_bar_->display_speed();
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

    update_seek_bar();
    redraw_frame();

    window_->show();
    update_title();
    clipping_version_ = clipping()->version();

    update_buffers(true);
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

void CutterWindow::button_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<CutterWindow *>(userdata);
    if (widget == window->btn_del_key_) {
        window->clipping_actions()->action_delete()();
    } else if (widget == window->btn_new_key_) {
        window->clipping_actions()->action_insert()();
    } else if (widget == window->btn_play_interval_) {
        window->action_play_interval();
    }
}

void CutterWindow::goto_selected_clipping_key() {
    if (!key_list_->value()) {
        return;
    }

    uint32_t index = key_list_->value() - 1;
    if (index < clipping()->keys().size()) {
        auto frame = clipping()->at_index(index).frame;

        player()->pause();
        player()->seek_frame(frame);
        update_buffers(true);
    }
}

void CutterWindow::video_list_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<CutterWindow *>(userdata);
    if (window->in_key_list_) {
        return;
    }
    window->in_key_list_ = true;
    auto selected_index = window->key_list_->value();
    if (selected_index) {
        window->key_list_->deselect();
        window->key_list_->select(selected_index);
        window->goto_selected_clipping_key();
    }
    window->in_key_list_ = false;
}

void CutterWindow::handle_clipping_resized() {
    redraw_frame();
    update_title();
}

void CutterWindow::action_play_interval() {
    if (!clipping()) {
        return;
    }

    if (clipping()->keys().empty()) {
        return;
    }

    player()->play(clipping()->first_frame(), clipping()->last_frame());
}

void CutterWindow::pause() {
    if (visible() && clipping()) {
        player()->pause();
    }
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
    update_buffers(true);
}

void CutterWindow::action_goto_reference() {
    uint32_t frame = 0;
    if (clipping()->ref().get_reference_frame(&frame)) {
        player()->pause();
        player()->seek_frame(frame);
        update_buffers(true);
    }
}

void CutterWindow::double_click(void *component) {
    if (component == viewer_) {
        clipping_actions()->action_properties()();
    }
}

void CutterWindow::update_clipping_list() {
    in_key_list_ = true;
    int selection = key_list_->value();
    uint32_t size = key_list_->size();

    key_list_->clear();
    uint32_t ref_frame = -1;
    clipping()->ref().get_reference_frame(&ref_frame);

    const ClippingKey *previous = NULL;
    for (const auto & k : clipping()->keys()) {
        key_list_->add((k.description(previous) + (ref_frame == k.frame ? " <r" : "")).c_str());
        previous = &k;
    }

    if (size == static_cast<uint32_t>(key_list_->size()) && selection != 0) {
        key_list_->deselect();
        key_list_->value(selection);
        key_list_->select(selection);
    }
    in_key_list_ = false;
}

void CutterWindow::redraw_frame(bool update_key_list) {
    if (update_key_list) {
        update_clipping_list();
        viewer_->update_preview(clipping());
    }

    clipping_editor_->redraw();
    viewer_->redraw();
    update_seek_bar();
}

void CutterWindow::update_seek_bar() {
    player_bar_->update();
}

void CutterWindow::key_list_auto_selection() {
    in_key_list_ = true;
    selected_clip_ = clipping()->find_index(player()->info()->position()) + 1;
    if (selected_clip_ > 0 && selected_clip_ <= static_cast<uint32_t>(key_list_->size())) {
        key_list_->value(selected_clip_);
    }
    in_key_list_ = false;
}

void CutterWindow::handle_buffer_modified() {
    update_buffers(true);
}

void CutterWindow::update_buffers(bool frame_changed) {
    if (player()->info()->error()) {
        return;
    }

    if (frame_changed) {
        update_seek_bar();
        redraw_frame();
        key_list_auto_selection();
        clipping_editor_->update(clipping());
        viewer_->update_preview(clipping());
    } else {
        clipping_editor_->draw_operations();
        if (clipping_editor_->key_changed(true)) {
            update_clipping_list();
            viewer_->update_preview(clipping());
        }
    }

    if (clipping()->w() < 15 || clipping()->h() < 15) {
        clipping()->wh(player()->info()->w(), player()->info()->h());
    }

    if (clipping()->keys().size() != static_cast<uint32_t>(key_list_->size())) {
        update_clipping_list();
    }
}

void CutterWindow::poll_actions() {
    if (clipping()) {
        update_buffers(player()->frame_changed(true));
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

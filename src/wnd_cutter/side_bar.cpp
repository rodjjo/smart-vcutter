/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/data/xpm.h"
#include "src/wnd_cutter/side_bar.h"
#include "src/wnd_cutter/player_bar.h"

namespace vcutter {

SideBar::SideBar(ClippingActions *actions, Fl_Group *parent, Fl_Widget *left_component) {
    in_key_list_ = false;
    selected_clip_ = 0;
    actions_ = actions;
    parent_ = parent;
    left_component_ = left_component;

    int parent_x = parent_->x();
    int parent_y = parent_->y();

    parent_->position(0, 0);

    btn_new_key_.reset(new Button(xpm::image(xpm::button_add), actions_->action_insert()));
    btn_del_key_.reset(new Button(xpm::image(xpm::button_delete), actions_->action_delete()));
    btn_play_interval_.reset(new Button(xpm::image(xpm::button_play), actions_->action_play_interval()));
    key_list_ = new Fl_Select_Browser(0, 30, 100, 100);
    viewer_ = new MiniatureViewer(0, 0, 100, 100);

    parent_->position(parent_x, parent_y);

    btn_new_key_->tooltip("[Insert] Insert a mark at current frame.");
    btn_del_key_->tooltip("[Delete] Remove the current frame mark.");
    btn_play_interval_->tooltip("Play the animation interval.");
    key_list_->callback(key_list_callback, this);
    viewer_->cursor(FL_CURSOR_HAND);
    btn_del_key_->shortcut(FL_Delete);
    btn_new_key_->shortcut(FL_Insert);
}

SideBar::~SideBar() {

}

void SideBar::update(bool check_size) {
    if (check_size && actions_->clipping()->keys().size() == static_cast<uint32_t>(key_list_->size())) {
        return;
    }

    if (!actions_->clipping()) {
        return;
    }

    in_key_list_ = true;

    int selection = key_list_->value();
    uint32_t size = key_list_->size();

    key_list_->clear();
    uint32_t ref_frame = -1;
    actions_->clipping()->ref().get_reference_frame(&ref_frame);

    for (const auto & k : actions_->clipping()->keys()) {
        key_list_->add((k.description() + (ref_frame == k.frame ? " <r" : "")).c_str());
    }

    if (size == static_cast<uint32_t>(key_list_->size()) && selection != 0) {
        key_list_->deselect();
        key_list_->value(selection);
        key_list_->select(selection);
    }

    in_key_list_ = false;
}

void SideBar::update_selection() {
    in_key_list_ = true;

    selected_clip_ = actions_->clipping()->find_index(actions_->clipping()->player()->info()->position()) + 1;
    if (selected_clip_ > 0 && selected_clip_ <= static_cast<uint32_t>(key_list_->size())) {
        key_list_->value(selected_clip_);
    }

    in_key_list_ = false;
}

int SideBar::default_width() {
    return 320;
}

void SideBar::resize_controls() {
    int parent_x = parent_->x();
    int parent_y = parent_->y();

    parent_->position(0, 0);

    int base_left = parent_->w() - default_width();
    static_cast<Fl_Widget *>(key_list_)->position(base_left, 33);
    int key_list_w = parent_->w() - base_left;
    int bottom_size = parent_->h() - left_component_->h();
    int key_list_size = left_component_->h() - key_list_w - bottom_size - 38;

    key_list_->size(key_list_w, key_list_size);
    int key_list_top = 33 + key_list_->h();

    viewer_->position(base_left, key_list_top + 1);

    btn_new_key_->position(base_left, 3);
    base_left += 27;
    btn_del_key_->position(base_left, 3);
    base_left += 27;
    btn_play_interval_->position(base_left, 3);


    viewer_->size(key_list_w, key_list_w);

    btn_new_key_->size(25, 25);
    btn_del_key_->size(25, 25);
    btn_play_interval_->size(25, 25);

    parent_->position(parent_x, parent_y);
}

MiniatureViewer *SideBar::viewer() {
    return viewer_;
}

void SideBar::key_list_callback(Fl_Widget* widget, void *userdata) {
    auto bar = static_cast<SideBar *>(userdata);

    if (bar->in_key_list_) {
        return;
    }

    bar->in_key_list_ = true;
    auto selected_index = bar->key_list_->value();
    if (selected_index) {
        bar->key_list_->deselect();
        bar->key_list_->select(selected_index);
        bar->goto_selected_clipping_key();
    }
    bar->in_key_list_ = false;
}

void SideBar::goto_selected_clipping_key() {
    if (!key_list_->value()) {
        return;
    }

    uint32_t index = key_list_->value() - 1;
    if (index < actions_->clipping()->keys().size()) {
        auto frame = actions_->clipping()->at_index(index).frame;
        actions_->clipping()->player()->pause();
        actions_->clipping()->player()->seek_frame(frame);
        actions_->handler()->handle_buffer_modified();
    }
}


}  // namespace vcutter

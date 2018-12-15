/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_CUTTER_SIDE_BAR_H_
#define SRC_WND_CUTTER_SIDE_BAR_H_

#include <memory>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Group.H>
#include "src/viewer/miniature_viewer.h"
#include "src/wnd_cutter/clipping_actions.h"
#include "src/ui/controls/button.h"

namespace vcutter {

class PlayerBar;

class SideBar {
 public:
    SideBar(ClippingActions *actions, Fl_Group *parent, Fl_Widget *left_component);
    virtual ~SideBar();
    void update_selection();
    void update(bool check_size = false);
    void resize_controls();
    MiniatureViewer *viewer();
    static int default_width();
 private:
    static void key_list_callback(Fl_Widget* widget, void *userdata);
    void goto_selected_clipping_key();

 private:
    bool in_key_list_;
    unsigned int selected_clip_;
    std::unique_ptr<Button> btn_new_key_;
    std::unique_ptr<Button> btn_del_key_;
    std::unique_ptr<Button> btn_play_interval_;
    Fl_Select_Browser *key_list_;
    Fl_Group *parent_;
    ClippingActions *actions_;
    Fl_Widget *left_component_;
    MiniatureViewer *viewer_;
};

}  // namespace vcutter

#endif  // SRC_WND_CUTTER_SIDE_BAR_H_

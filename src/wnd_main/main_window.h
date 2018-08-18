/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_MAIN_MAIN_WINDOW_H_
#define SRC_WND_MAIN_MAIN_WINDOW_H_

#include <memory>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

#include "src/data/project.h"
#include "src/wnd_cutter/cutter_window.h"
#include "src/data/history.h"
#include "src/wnd_main/menu.h"

namespace vcutter {


class MainWindow : Fl_Menu_Window {
 public:
    MainWindow();
    ~MainWindow();
    int run();
    void open_video_or_project(const std::string& path);
    void resize(int X, int Y, int W, int H) override;
 private:
    void init_main_menu();
    void init_tool_bar();
    void init_controls();
    void poll_actions();
    void load_sessions();

    void poll_key_repeat();
    bool should_handle_key(int value);
    void repeat_current_key();

    int handle(int event) override;
    static void timeout_handler(void* ud);
    static int default_window_width() ;
    static int default_window_height();
    static int default_window_left();
    static int default_window_top();
    static void prevent_close_cb(Fl_Widget *menu, void *this_window);

 private:
    void enable_controls();
    menu_callback_t action_file_open();
    menu_callback_t action_file_open_project();
    menu_callback_t action_file_save();
    menu_callback_t action_file_save_as();
    menu_callback_t action_file_generate();
    menu_callback_t action_file_close();
    menu_callback_t action_file_exit();
    menu_callback_t action_edit_copy();
    menu_callback_t action_edit_paste();
    menu_callback_t action_edit_paste_rotation();
    menu_callback_t action_edit_paste_scale();
    menu_callback_t action_edit_paste_position();
    menu_callback_t action_edit_paste_positionx();
    menu_callback_t action_edit_paste_positiony();
    menu_callback_t action_edit_clear_rotation();
    menu_callback_t action_edit_clear_scale();
    menu_callback_t action_edit_clear_position();
    menu_callback_t action_edit_scale_half();
    menu_callback_t action_edit_scale_half2();
    menu_callback_t action_edit_scale2();
    menu_callback_t action_edit_scale3();
    menu_callback_t action_use_ref();
    menu_callback_t action_edit_go_ref();
    menu_callback_t action_clear_ref();
    menu_callback_t action_transformation_swap_wh();
    menu_callback_t action_transformation_180();
    menu_callback_t action_transformation_scale();
    menu_callback_t action_rotation_90();
    menu_callback_t action_rotation_180();
    menu_callback_t action_rotation_270();
    menu_callback_t action_utils_convert();
    menu_callback_t action_utils_convert_current();
    menu_callback_t action_utils_clipping();

    menu_callback_t action_position_top();
    menu_callback_t action_position_left();
    menu_callback_t action_position_right();
    menu_callback_t action_position_bottom();
    menu_callback_t action_edit_properties();
    menu_callback_t action_edit_position_vertical();
    menu_callback_t action_edit_position_horizontal();
    menu_callback_t action_edit_align_top();
    menu_callback_t action_edit_align_bottom();
    menu_callback_t action_edit_align_left();
    menu_callback_t action_edit_align_right();
    menu_callback_t action_edit_align_all();
    menu_callback_t action_edit_norm_scale();
    menu_callback_t action_clear_keys();

    menu_callback_t action_create_ref();
    menu_callback_t action_edit_use_ref_rotation();
    menu_callback_t action_edit_use_ref_drag();
    menu_callback_t action_edit_use_ref_dragx();
    menu_callback_t action_edit_use_ref_dragy();
    menu_callback_t action_edit_use_ref_scale();
    menu_callback_t action_edit_use_ref_no_rotate();
    menu_callback_t action_edit_use_ref_no_scale();
    menu_callback_t action_edit_use_ref_no_drag();
    menu_callback_t action_toggle_compare_box();
    menu_callback_t action_wink_comparison();

    menu_callback_t action_about();
    menu_callback_t action_help();
    menu_callback_t action_tip();

    bool save_project(bool create_new_file);
    bool ask_for_save();

 private:
    bool run_called_;
    bool sessions_loaded_;
    uint64_t session_timelap_;
    uint64_t key_value_;
    uint64_t key_time_lap_;
 private:
    std::unique_ptr<CutterWindow> cutter_window_;
 private:
    History history_;
    Fl_Window *window_;
    Fl_Menu_Bar *menu_;
    Fl_Group *bottom_group_;
    std::unique_ptr<Project> project_;
    std::unique_ptr<Menu> menu_file_;
    std::unique_ptr<Menu> menu_edit_;
    std::unique_ptr<Menu> menu_tools_;
    std::unique_ptr<Menu> menu_utils_;
    std::unique_ptr<Menu> menu_help_;
};

}  // namespace vcutter

#endif  // SRC_WND_MAIN_MAIN_WINDOW_H_

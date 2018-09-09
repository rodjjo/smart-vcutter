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
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

#include "src/wnd_cutter/cutter_window.h"
#include "src/data/history.h"
#include "src/wnd_main/menu.h"
#include "src/wnd_main/menu_bar.h"

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
    void resize_controls();
    callback_t action_menu_popup();
    callback_t action_file_open();
    callback_t action_file_open_project();
    callback_t action_file_save();
    callback_t action_file_save_as();
    callback_t action_file_generate();
    callback_t action_file_close();
    callback_t action_file_exit();
    callback_t action_use_ref();
    callback_t action_edit_go_ref();
    callback_t action_clear_ref();
    callback_t action_utils_convert();
    callback_t action_utils_convert_current();
    callback_t action_utils_clipping();

    callback_t action_create_ref();
    callback_t action_edit_use_ref_rotation();
    callback_t action_edit_use_ref_drag();
    callback_t action_edit_use_ref_dragx();
    callback_t action_edit_use_ref_dragy();
    callback_t action_edit_use_ref_scale();
    callback_t action_edit_use_ref_no_rotate();
    callback_t action_edit_use_ref_no_scale();
    callback_t action_edit_use_ref_no_drag();
    callback_t action_toggle_compare_box();
    callback_t action_wink_comparison();

    callback_t action_about();
    callback_t action_help();
    callback_t action_tip();

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
    MenuBar *menu_;
    Menu *menu_compare_;
    Menu *menu_compare_alt_;
    Fl_Group *bottom_group_;
    std::unique_ptr<Menu> menu_file_;
    std::unique_ptr<Menu> menu_edit_;
    std::unique_ptr<Menu> menu_tools_;
    std::unique_ptr<Menu> menu_utils_;
    std::unique_ptr<Menu> menu_help_;
};

}  // namespace vcutter

#endif  // SRC_WND_MAIN_MAIN_WINDOW_H_

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <stdlib.h>
#include <memory>
#include <GL/gl.h>
#include <FL/Fl.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/fl_ask.H>


#ifdef _WIN32
#include <Windows.h>
#endif
#include "src/vstream/video_stream.h"
#include "src/wnd_main/main_window.h"
#include "src/wnd_tools/encoder_window.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/common/utils.h"

namespace vcutter {

namespace {

const int kMENU_HEIGHT = 25;
const float kTIMEOUT_INTERVAL = 0.0333;
const float kKEY_REPEAT_INTERVAL = 0.333;
}

#define GROUP_CLIPPING_OPEN 1

MainWindow::MainWindow() : Fl_Menu_Window(
    default_window_left(),
    default_window_top(),
    default_window_width(),
    default_window_height()
) {
    sessions_loaded_ = false;
    run_called_ = false;
    session_timelap_ = 0;
    key_value_ = 0;
    key_time_lap_ = 0;

    window_ = this;
    window_->size_range(default_window_width(), default_window_height());
    window_->begin();

    window_->color(fl_rgb_color(255, 255, 255));

    init_controls();

    window_->callback(prevent_close_cb, this);

    window_->end();
    window_->show();

    enable_controls();

    Fl::add_timeout(1.0, &MainWindow::timeout_handler, this);

    resize_controls();
}

MainWindow::~MainWindow() {
}

void MainWindow::resize_controls() {
    menu_->position(0, 0);
    menu_->size(window_->w(), kMENU_HEIGHT);
    bottom_group_->size(window_->w(), window_->h() - kMENU_HEIGHT);
    cutter_window_->resize_controls();
    bottom_group_->position(0, kMENU_HEIGHT);
}

void MainWindow::resize(int X, int Y, int W, int H) {
    Fl_Window::resize(X, Y, W, H);
    resize_controls();
}

void MainWindow::timeout_handler(void* ud) {
    static_cast<MainWindow *>(ud)->poll_actions();
    Fl::repeat_timeout(kTIMEOUT_INTERVAL, &MainWindow::timeout_handler, ud);
}

int MainWindow::run() {
    run_called_ = true;
    int result = Fl::run();
    cutter_window_.reset();
    return result;
}

void MainWindow::poll_actions() {
    poll_key_repeat();
    cutter_window_->poll_actions();
    load_sessions();
}

void MainWindow::load_sessions() {
    if (sessions_loaded_) {
        return;
    }
    sessions_loaded_ = true;

    if (cutter_window_->clipping_actions()->restore_session()) {
        enable_controls();
    }

    cutter_window_->clipping_actions()->action_pause()();
    EncoderWindow::restore_session(&history_, window_);
}

void MainWindow::init_tool_bar() {
    window_->begin();
    auto top_group = new Fl_Group(0, 0, default_window_width(), kMENU_HEIGHT, "");
    window_->end();

    top_group->align(FL_ALIGN_TOP);
    top_group->begin();
    top_group->box(FL_DOWN_BOX);
    init_main_menu();
    top_group->end();
}

void MainWindow::init_controls() {
    bottom_group_ = new Fl_Group(0, 0, window_->w(), window_->h() - kMENU_HEIGHT, "");
    bottom_group_->end();
    bottom_group_->color(fl_rgb_color(0, 0, 0));

    bottom_group_->begin();
    cutter_window_.reset(new CutterWindow(bottom_group_));
    bottom_group_->end();

    init_tool_bar();

    bottom_group_->position(0, kMENU_HEIGHT);
}

void MainWindow::init_main_menu() {
    menu_ = new MenuBar(default_window_width(), kMENU_HEIGHT, action_menu_popup());
    menu_->align(FL_ALIGN_TOP);

    menu_file_.reset(new Menu(menu_, "&File"));

    auto ca = cutter_window_->clipping_actions();

    menu_file_->add("&Open video", "^o", action_file_open(), 0, 0, xpm::film_16x16);
    menu_file_->add("Open &project", "^a", action_file_open_project(), 0, 0, xpm::directory_16x16);
    menu_file_->add("&Save project", "", action_file_save(), 0, GROUP_CLIPPING_OPEN, xpm::save_16x16);
    menu_file_->add("&Save project as", "^s", action_file_save_as(), 0, GROUP_CLIPPING_OPEN, xpm::save_as_16x16);
    menu_file_->add("&Save clipping", "^g", action_file_generate(), 0, GROUP_CLIPPING_OPEN, xpm::take_16x16);
    menu_file_->add("&Close", "", action_file_close(), 0, GROUP_CLIPPING_OPEN, xpm::eject_16x16);
    menu_file_->add("&Exit", "^x", action_file_exit(), 0, 0, xpm::exit_16x16);

    menu_edit_.reset(new Menu(menu_, "&Edit"));

    menu_edit_->add("&Copy", "^c", ca->action_copy(), 0, GROUP_CLIPPING_OPEN, xpm::copy_16x16);
    menu_edit_->add("Paste/Rotation", "", ca->action_paste_rotation(), 0, GROUP_CLIPPING_OPEN, xpm::rotate_16x16);
    menu_edit_->add("Paste/Scale", "", ca->action_paste_scale(), 0, GROUP_CLIPPING_OPEN, xpm::expand_16x16);
    menu_edit_->add("Paste/Position", "", ca->action_paste_position(true, true), 0, GROUP_CLIPPING_OPEN, xpm::move_16x16);
    menu_edit_->add("Paste/Position horiz.", "", ca->action_paste_position(true, false), 0, GROUP_CLIPPING_OPEN, xpm::left_right_16x16);
    menu_edit_->add("Paste/Position vert.", "", ca->action_paste_position(false, true), 0, GROUP_CLIPPING_OPEN, xpm::up_down_16x16);
    menu_edit_->add("&Paste", "^v", ca->action_paste(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN, xpm::paste_16x16);

    menu_edit_->add("Swap width and height", "", ca->action_swap_wh(), 0, GROUP_CLIPPING_OPEN, xpm::arrow_dwn_16x16);
    menu_edit_->add("Clear all keys", "", ca->action_clear_keys(), 0, GROUP_CLIPPING_OPEN, xpm::erase_all_16x16);
    menu_compare_ = menu_edit_->add("Compare first last frame", "", action_toggle_compare_box(), FL_MENU_TOGGLE, GROUP_CLIPPING_OPEN, xpm::eye_16x16);
    menu_compare_alt_ = menu_edit_->add("Alternate comparation", "", action_wink_comparison(), FL_MENU_DIVIDER | FL_MENU_TOGGLE, GROUP_CLIPPING_OPEN, xpm::clock_16x16);
    menu_edit_->add("Output properties", "", ca->action_properties(), 0, GROUP_CLIPPING_OPEN, xpm::note_16x16);

    menu_tools_.reset(new Menu(menu_, "&Tools"));
    menu_tools_->add("Magic rule/Create", "^m", action_create_ref(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN, xpm::magic_16x16);
    menu_tools_->add("Magic rule/Use", "^u", action_use_ref(), 0, GROUP_CLIPPING_OPEN, xpm::green_pin_16x16);
    menu_tools_->add("Magic rule/Use rotate only", "", action_edit_use_ref_rotation(), 0, GROUP_CLIPPING_OPEN, xpm::rotate_16x16);
    menu_tools_->add("Magic rule/Use positionate only", "", action_edit_use_ref_drag(), 0, GROUP_CLIPPING_OPEN, xpm::move_16x16);
    menu_tools_->add("Magic rule/Use positionate x only", "", action_edit_use_ref_dragx(), 0, GROUP_CLIPPING_OPEN, xpm::left_right_16x16);
    menu_tools_->add("Magic rule/Use positionate y only", "", action_edit_use_ref_dragy(), 0, GROUP_CLIPPING_OPEN, xpm::up_down_16x16);
    menu_tools_->add("Magic rule/Use scale only", "", action_edit_use_ref_scale(), 0, GROUP_CLIPPING_OPEN, xpm::expand_16x16);
    menu_tools_->add("Magic rule/Use no rotate", "", action_edit_use_ref_no_rotate(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Magic rule/Use no positionate", "", action_edit_use_ref_no_drag(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Magic rule/Use no scale", "^d", action_edit_use_ref_no_scale(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Magic rule/Go to frame", "", action_edit_go_ref(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Magic rule/Clear", "", action_clear_ref(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Rotation/Clear", "#r", ca->action_clear_rotation(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Rotation/90 degrees", "", ca->action_rotation_90(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Rotation/180 degrees", "", ca->action_rotation_180(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Rotation/270 degrees", "", ca->action_rotation_270(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/1.0x Scale", "#s", ca->action_clear_scale(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/0.5x Scale", "", ca->action_scale_half(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/0.25x Scale", "", ca->action_scale_half_2(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/2.0x Scale", "", ca->action_scale_2(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/3.0x Scale", "", ca->action_scale_3(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/Normalize", "", ca->action_norm_scale(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Top", "", ca->action_position_top(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Left", "", ca->action_position_left(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Right", "", ca->action_position_right(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/bottom", "", ca->action_position_bottom(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Centralize", "#c", ca->action_clear_position(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Center Vertical", "", ca->action_position_vertical(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Ceter Horizontal", "", ca->action_position_horizontal(), 0, GROUP_CLIPPING_OPEN);

    menu_tools_->add("Align/Top", "", ca->action_align_top(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Align/bottom", "", ca->action_align_bottom(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Align/Left", "", ca->action_align_left(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Align/Right", "", ca->action_align_right(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Align/All", "", ca->action_align_all(), 0, GROUP_CLIPPING_OPEN);

    menu_tools_->add("All keys/Rotate +180 degree", "", ca->action_rotate_all_180(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("All keys/Scale", "", ca->action_scale_all(), 0, GROUP_CLIPPING_OPEN);

    menu_utils_.reset(new Menu(menu_, "Utils"));

    menu_utils_->add("Convert video", "^j", action_utils_convert(), 0, 0, xpm::film_16x16);
    menu_utils_->add("Convert clipping", "^p", action_utils_clipping(), 0, 0, xpm::directory_16x16);
    menu_utils_->add("Convert current video", "^p", action_utils_convert_current(), 0, GROUP_CLIPPING_OPEN, xpm::cd_16x16);

    menu_help_.reset(new Menu(menu_, "Help"));
    menu_help_->add("About", "", action_about(), 0, 0, xpm::smile_16x16);
    menu_help_->add("Show tips", "", action_tip(), 0, 0, xpm::hint_16x16);
    menu_help_->add("Get help", "", action_help(), 0, 0, xpm::help_16x16);
}

callback_t MainWindow::action_menu_popup() {
    return [this] () {
        menu_compare_alt_->enable(cutter_window_->visible() && cutter_window_->compare_enabled());
        menu_compare_->check(cutter_window_->compare_enabled());
        menu_compare_alt_->check(cutter_window_->compare_alternate());
    };
}

callback_t MainWindow::action_utils_clipping() {
    return [this] () {
        const char *key = "main-window-project-dir";
        std::string directory = history_[key];
        std::string path = input_prj_file_chooser(&directory);

        if (!directory.empty()) {
            history_.set(key, directory.c_str());
        }

        if (path.empty()) {
            return;
        }

        std::shared_ptr<Clipping> clip(new Clipping(path.c_str(), false));
        if (!clip->good()) {
            show_error("Could not open the clipping project.");
            return;
        }

        cutter_window_->clipping_actions()->action_pause()();
        EncoderWindow::execute(&history_, window_, clip);
    };
}

callback_t MainWindow::action_utils_convert_current() {
    return [this] () {
        if (!cutter_window_->visible()) {
            return;
        }

        cutter_window_->clipping_actions()->action_pause()();
        EncoderWindow::execute(&history_, window_, cutter_window_->get_video_path());
    };
}

callback_t MainWindow::action_file_close() {
    return [this] () {
        if (ask_for_save()) {
            cutter_window_->close();
        }

        enable_controls();
    };
}

callback_t MainWindow::action_file_generate() {
    return [this] () {
        if (cutter_window_->visible()) {
            cutter_window_->clipping_actions()->action_pause()();
            EncoderWindow::execute(&history_, window_, cutter_window_->to_clipping());
        }
    };
}

int MainWindow::default_window_width() {
  int value = 1280;
  if (value + 50 > Fl::w()) {
    value = Fl::w() - 50;
  }
  return value;
}

int MainWindow::default_window_height() {
  int value = 720;
  if (value + 100 > Fl::h()) {
    value = Fl::h() - 100;
  }
  return value;
}

int MainWindow::default_window_left() {
  return (Fl::w() - default_window_width()) / 2;
}

int MainWindow::default_window_top() {
  return (Fl::h() - default_window_height()) / 2;
}

void MainWindow::enable_controls() {
    bool enabled = cutter_window_->visible();
    menu_utils_->enable(enabled, GROUP_CLIPPING_OPEN);
    menu_file_->enable(enabled, GROUP_CLIPPING_OPEN);
    menu_edit_->enable(enabled, GROUP_CLIPPING_OPEN);
    menu_tools_->enable(enabled, GROUP_CLIPPING_OPEN);
}


void MainWindow::open_video_or_project(const std::string& path) {
    if (path.empty()) {
        return;
    }

    std::string extension(".vcutter");
    if (path.substr(path.size() - extension.size()) == extension) {
        if (cutter_window_->clipping_actions()->open(path, false)) {
            enable_controls();
            return;
        }
    } else {
        if (cutter_window_->clipping_actions()->open(path, true)) {
            enable_controls();
            return;
        }
    }
}


callback_t MainWindow::action_file_open_project() {
    return [this] () {
        if (!ask_for_save()) {
            return;
        }
        const char *key = "main-window-project-dir";
        std::string directory = history_[key];
        open_video_or_project(input_prj_file_chooser(&directory));
        if (!directory.empty()) {
            history_.set(key, directory.c_str());
        }
        enable_controls();
    };
}

callback_t MainWindow::action_file_open() {
    return [this] () {
        if (!ask_for_save()) {
            return;
        }
        const char *key = "main-window-video-dir";
        std::string directory = history_[key];
        open_video_or_project(input_video_file_chooser(&directory));
        if (!directory.empty()) {
            history_.set(key, directory.c_str());
        }
        enable_controls();
    };
}

bool MainWindow::save_project(bool create_new_file) {
    if (!cutter_window_->visible()) {
        return false;
    }

    if (create_new_file) {
        return cutter_window_->clipping_actions()->save_as(&history_);
    }

    return cutter_window_->clipping_actions()->save(&history_);
}

callback_t MainWindow::action_file_save() {
    return [this] () {
        save_project(false);
    };
}

callback_t MainWindow::action_file_save_as() {
    return [this] () {
        save_project(true);
    };
}

callback_t MainWindow::action_create_ref() {
    return [this] () {
        cutter_window_->action_create_ref();
    };
}

callback_t MainWindow::action_toggle_compare_box() {
    return [this] () {
        cutter_window_->action_toggle_compare();
    };
}

callback_t MainWindow::action_wink_comparison() {
    return [this] () {
        cutter_window_->action_toggle_compare_wink();
    };
}

callback_t MainWindow::action_use_ref() {
    return [this] () {
        cutter_window_->action_use_ref(true, true, true, true);
    };
}

callback_t MainWindow::action_edit_use_ref_rotation() {
    return [this] () {
        cutter_window_->action_use_ref(false, false, true, false);
    };
}

callback_t MainWindow::action_edit_use_ref_drag() {
    return [this] () {
        cutter_window_->action_use_ref(true, true, false, false);
    };
}

callback_t MainWindow::action_edit_use_ref_dragx() {
    return [this] () {
        cutter_window_->action_use_ref(true, false, false, false);
    };
}

callback_t MainWindow::action_edit_use_ref_dragy() {
    return [this] () {
        cutter_window_->action_use_ref(false, true, false, false);
    };
}

callback_t MainWindow::action_edit_use_ref_scale() {
    return [this] () {
        cutter_window_->action_use_ref(false, false, false, true);
    };
}

callback_t MainWindow::action_edit_use_ref_no_rotate() {
    return [this] () {
        cutter_window_->action_use_ref(true, true, false, true);
    };
}

callback_t MainWindow::action_edit_use_ref_no_scale() {
    return [this] () {
        cutter_window_->action_use_ref(true, true, true, false);
    };
}

callback_t MainWindow::action_edit_use_ref_no_drag() {
    return [this] () {
        cutter_window_->action_use_ref(false, false, true, true);
    };
}

callback_t MainWindow::action_edit_go_ref() {
    return [this] () {
        cutter_window_->action_goto_reference();
    };
}

callback_t MainWindow::action_clear_ref() {
    return [this] () {
        cutter_window_->action_clear_ref();
    };
}

bool MainWindow::ask_for_save() {
    if (cutter_window_->modified()) {
        switch (yes_nc("Do you want to save the project changes ?")) {
            case 0:
                if (!save_project(false)) {
                    return false;
                }
        break;
            case 1:
                return false;
            break;
        }

    }

    return true;
}

callback_t MainWindow::action_file_exit() {
    return [this] () {
        if (!ask_for_save()) {
            return;
        }
        Fl::delete_widget(window_);
    };
}

void MainWindow::prevent_close_cb(Fl_Widget *menu, void *this_window) {
    if (!static_cast<MainWindow *>(this_window)->ask_for_save()) {
        return;
    }
    static_cast<MainWindow *>(this_window)->window_->hide();
}

callback_t MainWindow::action_utils_convert() {
    return [this] () {
        cutter_window_->clipping_actions()->action_pause()();
        EncoderWindow::execute(&history_, window_);
    };
}

callback_t MainWindow::action_about() {
    return [this] () {
        show_error("Rodrigo A. Araujo\nrodjjo@@gmail.com");
    };
}

callback_t MainWindow::action_help() {
    return [this] () {
        show_error("The help manual is in development");
    };
}

callback_t MainWindow::action_tip() {
    return [this] () {
        show_error("The tips window will be available soon");
    };
}

void MainWindow::poll_key_repeat() {
    if (!key_value_) {
        return;
    }

    if (key_time_lap_ >= (kKEY_REPEAT_INTERVAL * 1000)) {
        repeat_current_key();
        return;
    }

    key_time_lap_ += (kTIMEOUT_INTERVAL * 1000);
}

void MainWindow::repeat_current_key() {
    key_time_lap_ = 0;
    switch(key_value_) {
        case FL_Right: {
            cutter_window_->clipping_actions()->action_next()();
        }
        break;

        case FL_Left: {
            cutter_window_->clipping_actions()->action_prior()();
        }
        break;
    }
}

bool MainWindow::should_handle_key(int value) {
    switch(value) {
        case FL_Right:
        case FL_Left:
            return true;
    }
    return false;
}

int MainWindow::handle(int event) {
    switch (event) {
        case FL_KEYUP: {
            if (Fl::event_key() == FL_Escape) {
                return  1;
            }
            if (should_handle_key(Fl::event_key())) {
                repeat_current_key();
                key_value_ = 0;
                return 1;
            }
            if (Fl::event_key() == 32) {
                cutter_window_->clipping_actions()->action_pause_resume()();
                return 1;
            }
        }
        break;
        case FL_KEYDOWN: {
            if (Fl::event_key() == FL_Escape) {
                cutter_window_->cancel_operations();
                return 1;
            }
            if (should_handle_key(Fl::event_key())) {
                key_value_ = Fl::event_key();
                return 1;
            }
        }
        break;
    }

    return Fl_Menu_Window::handle(event);
}

}  // namespace vcutter

#ifndef UNIT_TEST

int main(int argc, char **argv) {
    vs::initialize();
    Fl::scheme("gtk+");

    auto main_window = new vcutter::MainWindow();

    if (argc > 1) {
        main_window->open_video_or_project(argv[1]);
    }

    int result = main_window->run();

    return result;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
     return main(__argc, __argv);
}
#endif

#endif  // UNIT_TEST

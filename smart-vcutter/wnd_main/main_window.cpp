/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <stdlib.h>
#include <memory>
#include <mutex>
#include <chrono>
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
#include "video_stream/vstream/video_stream.h"
#include "smart-vcutter/wnd_main/main_window.h"
#include "smart-vcutter/wnd_tools/encoder_window.h"
#include "smart-vcutter/wnd_common/common_dialogs.h"
#include "smart-vcutter/common/utils.h"

namespace vcutter {

namespace {
const char *kSESSION_SUFIX = "mwclp";
const char *kSESSION_PATH = "project_path";
const char *kSESSION_DATA = "project_data";

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
    project_.reset(new Project());
    clipping_session_.reset(new Session(kSESSION_SUFIX));

    window_ = this;
    window_->size_range(default_window_width(), default_window_height());
    window_->begin();

    window_->color(fl_rgb_color(255, 255, 255));

    init_tool_bar();

    window_->callback(prevent_close_cb, this);

    init_controls();

    window_->end();
    window_->show();

    enable_controls();


    Fl::add_timeout(1.0, &MainWindow::timeout_handler, this);
}

MainWindow::~MainWindow() {
}

void MainWindow::resize(int X, int Y, int W, int H) {
    Fl_Window::resize(X, Y, W, H);
    menu_->size(W,kMENU_HEIGHT);
    bottom_group_->size(window_->w(), window_->h() -kMENU_HEIGHT);
    cutter_window_->resize_controls();
}

void MainWindow::timeout_handler(void* ud) {
    static_cast<MainWindow *>(ud)->poll_actions();
    Fl::repeat_timeout(kTIMEOUT_INTERVAL, &MainWindow::timeout_handler, ud);
}

int MainWindow::run() {
    run_called_ = true;
    Fl::run();
    close_session();
}

void MainWindow::poll_actions() {
    poll_key_repeat();
    cutter_window_->poll_actions();
    load_sessions();
    save_session();
}

void MainWindow::load_sessions() {
    if (sessions_loaded_) {
        return;
    }
    sessions_loaded_ = true;

    if (clipping_session_->loaded()) {
        std::unique_ptr<Project> project(new Project());
        const auto & root = clipping_session_->get_data();

        if (project->open(root[kSESSION_DATA], root[kSESSION_PATH].asString())) {
            auto clipping = project->get_clipping();
            if (clipping && cutter_window_->open_clipping(*clipping)) {
                project_.swap(project);
                enable_controls();
            }
        }
    }

    cutter_window_->pause();
    EncoderWindow::restore_session(&history_, window_);
}

void MainWindow::save_session() {
    if (!cutter_window_->visible()) {
        if (clipping_session_->saved()) {
            close_session();
        }
    }

    if (!cutter_window_->visible() ||
        cutter_window_->modified_version() == clipping_session_->get_version()) {
        return;
    }

    session_timelap_ += (1000 * kTIMEOUT_INTERVAL);
    if (session_timelap_ < 5000) {
        return;
    }

    session_timelap_ = 0;

    Project backup;
    backup.set_clipping(cutter_window_->to_clipping());

    Json::Value root;
    root[kSESSION_PATH] = project_->get_path();
    root[kSESSION_DATA] = backup.get_data();

    clipping_session_->save(cutter_window_->modified_version(), root);
}

void MainWindow::close_session() {
    clipping_session_.reset(); // uses the destructor before the constructor
    clipping_session_.reset(new Session(kSESSION_SUFIX));
}

void MainWindow::init_tool_bar() {
    auto top_group = new Fl_Group(0, 0, default_window_width(), kMENU_HEIGHT, "");
    top_group->align(FL_ALIGN_TOP);
    top_group->begin();
    top_group->box(FL_DOWN_BOX);

    init_main_menu();
    top_group->end();
}

void MainWindow::init_controls() {
    bottom_group_ = new Fl_Group(0, kMENU_HEIGHT, window_->w(), window_->h() -kMENU_HEIGHT, "");
    bottom_group_->end();
    bottom_group_->color(fl_rgb_color(0, 0, 0));
    cutter_window_.reset(new CutterWindow(bottom_group_));
}

void MainWindow::init_main_menu() {
    menu_ = new Fl_Menu_Bar(0, 0, default_window_width(), kMENU_HEIGHT);
    menu_->align(FL_ALIGN_TOP);

    menu_file_.reset(new Menu(menu_, "&File"));
    menu_file_->add("&Open video", "^o", action_file_open(), 0, 0, xpm::film_16x16);
    menu_file_->add("Open &project", "^a", action_file_open_project(), 0, 0, xpm::directory_16x16);
    menu_file_->add("&Save project", "", action_file_save(), 0, GROUP_CLIPPING_OPEN, xpm::save_16x16);
    menu_file_->add("&Save project as", "^s", action_file_save_as(), 0, GROUP_CLIPPING_OPEN, xpm::save_as_16x16);
    menu_file_->add("&Save clipping", "^g", action_file_generate(), 0, GROUP_CLIPPING_OPEN, xpm::take_16x16);
    menu_file_->add("&Close", "^n", action_file_close(), 0, GROUP_CLIPPING_OPEN, xpm::eject_16x16);
    menu_file_->add("&Exit", "^x", action_file_exit(), 0, 0, xpm::exit_16x16);

    menu_edit_.reset(new Menu(menu_, "&Edit"));

    menu_edit_->add("&Copy", "^c", action_edit_copy(), 0, GROUP_CLIPPING_OPEN, xpm::copy_16x16);
    menu_edit_->add("Paste/Rotation", "", action_edit_paste_rotation(), 0, GROUP_CLIPPING_OPEN, xpm::rotate_16x16);
    menu_edit_->add("Paste/Scale", "", action_edit_paste_scale(), 0, GROUP_CLIPPING_OPEN, xpm::expand_16x16);
    menu_edit_->add("Paste/Position", "", action_edit_paste_position(), 0, GROUP_CLIPPING_OPEN, xpm::move_16x16);
    menu_edit_->add("Paste/Position horiz.", "", action_edit_paste_positionx(), 0, GROUP_CLIPPING_OPEN, xpm::left_right_16x16);
    menu_edit_->add("Paste/Position vert.", "", action_edit_paste_positiony(), 0, GROUP_CLIPPING_OPEN, xpm::up_down_16x16);
    menu_edit_->add("&Paste", "^v", action_edit_paste(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN, xpm::paste_16x16);

    menu_edit_->add("Swap width and height", "", action_transformation_swap_wh(), 0, GROUP_CLIPPING_OPEN, xpm::arrow_dwn_16x16);
    menu_edit_->add("Clear all keys", "", action_clear_keys(), 0, GROUP_CLIPPING_OPEN, xpm::erase_all_16x16);
    menu_edit_->add("Compare first last frame", "", action_toggle_compare_box(), 0, GROUP_CLIPPING_OPEN, xpm::eye_16x16);
    menu_edit_->add("Alternate comparation", "", action_wink_comparison(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN, xpm::clock_16x16);
    menu_edit_->add("Output properties", "", action_edit_properties(), 0, GROUP_CLIPPING_OPEN, xpm::note_16x16);

    // TODO(add alignment function in clipping keeper):
    // menu_edit_->add("Align/Top", "", action_edit_align_top(), 0, GROUP_CLIPPING_OPEN);
    // menu_edit_->add("Align/bottom", "", action_edit_align_bottom(), 0, GROUP_CLIPPING_OPEN);
    // menu_edit_->add("Align/Left", "", action_edit_align_left(), 0, GROUP_CLIPPING_OPEN);
    // menu_edit_->add("Align/Right", "", action_edit_align_right(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN);
    // menu_edit_->add("Align/All", "", action_edit_align_all(), 0, GROUP_CLIPPING_OPEN);


    menu_tools_.reset(new Menu(menu_, "&Tools"));
    menu_tools_->add("Magic rule/Create", "^m", action_create_ref(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN, xpm::magic_16x16);
    menu_tools_->add("Magic rule/Use", "^g", action_use_ref(), 0, GROUP_CLIPPING_OPEN, xpm::green_pin_16x16);
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
    menu_tools_->add("Rotation/Clear", "#r", action_edit_clear_rotation(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Rotation/90 degrees", "", action_rotation_90(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Rotation/180 degrees", "", action_rotation_180(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Rotation/270 degrees", "", action_rotation_270(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/1.0x Scale", "#s", action_edit_clear_scale(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/0.5x Scale", "", action_edit_scale_half(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/0.25x Scale", "", action_edit_scale_half2(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/2.0x Scale", "", action_edit_scale2(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/3.0x Scale", "", action_edit_scale3(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Scale/Normalize", "", action_edit_norm_scale(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Top", "", action_position_top(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Left", "", action_position_left(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Right", "", action_position_right(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/bottom", "", action_position_bottom(), FL_MENU_DIVIDER, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Centralize", "#c", action_edit_clear_position(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Center Vertical", "", action_edit_position_vertical(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("Position/Ceter Horizontal", "", action_edit_position_horizontal(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("All keys/Rotate +180 degree", "", action_transformation_180(), 0, GROUP_CLIPPING_OPEN);
    menu_tools_->add("All keys/Scale", "", action_transformation_scale(), 0, GROUP_CLIPPING_OPEN);


    menu_utils_.reset(new Menu(menu_, "Utils"));

    menu_utils_->add("Convert video", "^j", action_utils_convert(), 0, 0, xpm::film_16x16);
    menu_utils_->add("Convert clipping", "^p", action_utils_clipping(), 0, 0, xpm::directory_16x16);
    menu_utils_->add("Convert current video", "^p", action_utils_convert_current(), 0, GROUP_CLIPPING_OPEN, xpm::cd_16x16);

    menu_help_.reset(new Menu(menu_, "Help"));
    menu_help_->add("About", "", action_about(), 0, 0, xpm::smile_16x16);
    menu_help_->add("Show tips", "", action_tip(), 0, 0, xpm::hint_16x16);
    menu_help_->add("Get help", "", action_help(), 0, 0, xpm::help_16x16);
}

menu_callback_t MainWindow::action_utils_clipping() {
    return [this] (Menu *) {
        const char *key = "main-window-project-dir";
        std::string directory = history_.get(key);
        std::string path = input_prj_file_chooser(&directory);
        if (!directory.empty()) {
            history_.set(key, directory.c_str());
        }
        if (path.empty()) {
            return;
        }

        Project tmp(path) ;
        auto clipping = tmp.get_clipping();
        if (!clipping) {
            show_error(tmp.get_last_error().c_str());
            return;
        }

        cutter_window_->pause();
        EncoderWindow::execute(&history_, window_, *clipping);
    };
}

menu_callback_t MainWindow::action_utils_convert_current() {
    return [this] (Menu *) {
        if (!cutter_window_->visible()) {
            return;
        }

        cutter_window_->pause();
        EncoderWindow::execute(&history_, window_, cutter_window_->get_video_path());
    };
}



menu_callback_t MainWindow::action_edit_copy() {
    return [this] (Menu*) {
        cutter_window_->copy();
    };
}

menu_callback_t MainWindow::action_edit_paste() {
    return [this] (Menu*) {
        cutter_window_->past();
    };
}

menu_callback_t MainWindow::action_edit_paste_rotation() {
    return [this] (Menu*) {
        cutter_window_->past_rotation();
    };
}

menu_callback_t MainWindow::action_edit_paste_scale() {
    return [this] (Menu*) {
        cutter_window_->past_scale();
    };
}

menu_callback_t MainWindow::action_edit_paste_position() {
    return [this] (Menu*) {
        cutter_window_->past_position(true, true);
    };
}

menu_callback_t MainWindow::action_edit_paste_positionx() {
    return [this] (Menu*) {
        cutter_window_->past_position(true, false);
    };
}

menu_callback_t MainWindow::action_edit_paste_positiony() {
    return [this] (Menu*) {
        cutter_window_->past_position(false, true);
    };
}


menu_callback_t MainWindow::action_edit_clear_rotation() {
    return [this] (Menu*) {
        cutter_window_->clear_rotation();
    };
}

menu_callback_t MainWindow::action_edit_clear_scale() {
    return [this] (Menu*) {
        cutter_window_->clear_scale();
    };
}

menu_callback_t MainWindow::action_edit_clear_position() {
    return [this] (Menu*) {
        cutter_window_->clear_position();
    };
}

menu_callback_t MainWindow::action_edit_scale_half() {
    return [this] (Menu*) {
        cutter_window_->scale_half();
    };
}

menu_callback_t MainWindow::action_edit_scale_half2() {
    return [this] (Menu*) {
        cutter_window_->scale_half_2();
    };
}

menu_callback_t MainWindow::action_edit_scale2() {
    return [this] (Menu*) {
        cutter_window_->scale_2();
    };
}

menu_callback_t MainWindow::action_edit_scale3() {
    return [this] (Menu*) {
        cutter_window_->scale_3();
    };
}

menu_callback_t MainWindow::action_rotation_90() {
    return [this] (Menu*) {
        cutter_window_->rotation_90();
    };
}

menu_callback_t MainWindow::action_rotation_180() {
    return [this] (Menu*) {
        cutter_window_->rotation_180();
    };
}

menu_callback_t MainWindow::action_rotation_270() {
    return [this] (Menu*) {
        cutter_window_->rotation_270();
    };
}

menu_callback_t MainWindow::action_transformation_swap_wh() {
    return [this] (Menu*) {
        cutter_window_->swap_wh();
    };
}

menu_callback_t MainWindow::action_transformation_180() {
    return [this] (Menu*) {
        cutter_window_->rotate_all_180();
    };
}

menu_callback_t MainWindow::action_transformation_scale() {
    return [this] (Menu*) {
        cutter_window_->scale_all();
    };
}

menu_callback_t MainWindow::action_file_close() {
    return [this] (Menu*) {
        if (ask_for_save()) {
            cutter_window_->close();
        }

        enable_controls();
    };
}

menu_callback_t MainWindow::action_file_generate() {
    return [this] (Menu*) {
        if (cutter_window_->visible()) {
            cutter_window_->pause();
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
        std::unique_ptr<Project> tmp(new Project(path));

        auto clipping = tmp->get_clipping();

        if (!clipping) {
            show_error(tmp->get_last_error().c_str());
            return;
        }

        if (cutter_window_->open_clipping(*clipping)) {
            project_.swap(tmp);
            project_->set_clipping(cutter_window_->to_clipping());
            enable_controls();
            return;
        }
    } else {
        project_.reset(new Project());
        if (cutter_window_->open_video(path)) {
            project_->set_clipping(cutter_window_->to_clipping());
        }
    }

    enable_controls();
}


menu_callback_t MainWindow::action_file_open_project() {
    return [this] (Menu*) {
        if (!ask_for_save()) {
            return;
        }
        const char *key = "main-window-project-dir";
        std::string directory = history_.get(key);
        open_video_or_project(input_prj_file_chooser(&directory));
        if (!directory.empty()) {
            history_.set(key, directory.c_str());
        }
        enable_controls();
    };
}

menu_callback_t MainWindow::action_file_open() {
    return [this] (Menu*) {
        if (!ask_for_save()) {
            return;
        }
        const char *key = "main-window-video-dir";
        std::string directory = history_.get(key);
        open_video_or_project(input_video_file_chooser(&directory));
        if (!directory.empty()) {
            history_.set(key, directory.c_str());
        }
        enable_controls();
    };
}

bool MainWindow::save_project(bool create_new_file) {
    std::string path = create_new_file ? std::string() : project_->get_path();

    if (path.empty()) {
        const char *key = "main-window-project-dir";
        std::string directory = history_.get(key);
        path = output_prj_file_chooser(&directory);
        if (!directory.empty()) {
            history_.set(key, directory.c_str());
        }
        if (path.empty()) {
            return false;
        }
    }

    if (create_new_file && filepath_exists(path.c_str()) && !ask("The file already exists. Overwrite it ?")) {
        return false;
    }

    project_->set_clipping(cutter_window_->to_clipping());
    project_->save(path);
    cutter_window_->clear_modified();

    return true;
}

menu_callback_t MainWindow::action_file_save() {
    return [this] (Menu*) {
        save_project(false);
    };
}

menu_callback_t MainWindow::action_file_save_as() {
    return [this] (Menu*) {
        save_project(true);
    };
}

menu_callback_t MainWindow::action_create_ref() {
    return [this] (Menu*) {
        cutter_window_->action_create_ref();
    };
}

menu_callback_t MainWindow::action_toggle_compare_box() {
    return [this] (Menu*) {
        cutter_window_->action_toggle_compare();
    };
}

menu_callback_t MainWindow::action_wink_comparison() {
    return [this] (Menu*) {
        cutter_window_->action_toggle_compare_wink();
    };
}

menu_callback_t MainWindow::action_use_ref() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(true, true, true, true);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_rotation() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(false, false, true, false);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_drag() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(true, true, false, false);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_dragx() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(true, false, false, false);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_dragy() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(false, true, false, false);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_scale() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(false, false, false, true);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_no_rotate() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(true, true, false, true);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_no_scale() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(true, true, true, false);
    };
}

menu_callback_t MainWindow::action_edit_use_ref_no_drag() {
    return [this] (Menu*) {
        cutter_window_->action_use_ref(false, false, true, true);
    };
}

menu_callback_t MainWindow::action_edit_go_ref() {
    return [this] (Menu*) {
        cutter_window_->action_goto_reference();
    };
}

menu_callback_t MainWindow::action_clear_ref() {
    return [this] (Menu*) {
        cutter_window_->action_clear_ref();
    };
}

menu_callback_t MainWindow::action_edit_properties() {
    return [this] (Menu*) {
        cutter_window_->action_properties();
    };
}

menu_callback_t MainWindow::action_position_top() {
    return [this] (Menu*) {
        cutter_window_->action_position_top();
    };
}

menu_callback_t MainWindow::action_position_left() {
    return [this] (Menu*) {
        cutter_window_->action_position_left();
    };
}

menu_callback_t MainWindow::action_position_right() {
    return [this] (Menu*) {
        cutter_window_->action_position_right();
    };
}

menu_callback_t MainWindow::action_position_bottom() {
    return [this] (Menu*) {
        cutter_window_->action_position_bottom();
    };
}

menu_callback_t MainWindow::action_edit_position_vertical() {
    return [this] (Menu*) {
        cutter_window_->action_position_vertical();
    };
}

menu_callback_t MainWindow::action_edit_position_horizontal() {
    return [this] (Menu*) {
        cutter_window_->action_position_horizontal();
    };
}

menu_callback_t MainWindow::action_edit_align_top() {
    return [this] (Menu*) {
        cutter_window_->action_align_top();
    };
}

menu_callback_t MainWindow::action_edit_align_bottom() {
    return [this] (Menu*) {
        cutter_window_->action_align_bottom();
    };
}

menu_callback_t MainWindow::action_edit_align_left() {
    return [this] (Menu*) {
        cutter_window_->action_align_left();
    };
}

menu_callback_t MainWindow::action_edit_align_right() {
    return [this] (Menu*) {
        cutter_window_->action_align_right();
    };
}

menu_callback_t MainWindow::action_edit_align_all() {
    return [this] (Menu*) {
        cutter_window_->action_align_all();
    };
}

menu_callback_t MainWindow::action_edit_norm_scale() {
    return [this] (Menu*) {
        cutter_window_->action_norm_scale();
    };
}

menu_callback_t MainWindow::action_clear_keys() {
    return [this] (Menu*) {
        cutter_window_->action_clear_keys();
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

menu_callback_t MainWindow::action_file_exit() {
    return [this] (Menu *) {
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

menu_callback_t MainWindow::action_utils_convert() {
    return [this] (Menu *) {
        cutter_window_->pause();
        EncoderWindow::execute(&history_, window_);
    };
}

menu_callback_t MainWindow::action_about() {
    return [this] (Menu *) {
        show_error("Rodrigo A. Araujo\nrodjjo@@gmail.com");
    };
}

menu_callback_t MainWindow::action_help() {
    return [this] (Menu *) {
        show_error("The help manual is in development");
    };
}

menu_callback_t MainWindow::action_tip() {
    return [this] (Menu *) {
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
            cutter_window_->next();
        }
        break;

        case FL_Left: {
            cutter_window_->prior();
        }
        break;
    };
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
                cutter_window_->pause_resume();
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

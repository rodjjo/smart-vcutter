/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_WND_CUTTER_CUTTER_WINDOW_H_
#define SMART_VCUTTER_WND_CUTTER_CUTTER_WINDOW_H_

#include <inttypes.h>
#include <set>
#include <string>
#include <memory>
#include <atomic>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Select_Browser.H>

#include "smart-vcutter/data/project.h"
#include "smart-vcutter/wnd_cutter/options_window.h"
#include "smart-vcutter/wrappers/video_player.h"
#include "smart-vcutter/clippings/clipping_painter.h"
#include "smart-vcutter/viewer/miniature_viewer.h"
#include "smart-vcutter/viewer/editor/clipping_editor.h"



namespace vcutter {

class CutterWindow {
 public:
    CutterWindow(Fl_Group *parent);
    virtual ~CutterWindow();
    bool open_clipping(const clipping_t & clip);
    bool open_video(const std::string& video_path);
    void close();
    clipping_t to_clipping();
    void poll_actions();
    bool visible();
    bool modified();
    uint64_t modified_version();
    void clear_modified();

    void cancel_operations();

    std::string get_video_path();
    bool is_paused(bool show_message=false);
    void pause();
    bool has_copy();
    bool has_copy(const char* paused_message);
    void no_copy();
    void copy();
    void past();
    void past_rotation();
    void past_scale();
    void past_position(bool x, bool y);
    void clear_rotation();
    void clear_scale();
    void scale_half();
    void scale_half_2();
    void scale_2();
    void scale_3();
    void clear_position();
    void rotation_90();
    void rotation_180();
    void rotation_270();
    void swap_wh();
    void rotate_all_180();
    void pause_resume();
    void scale_all();
    void prior();
    void next();

    void action_goto_reference();
    void action_create_ref();
    void action_use_ref(bool positionate_x, bool positionate_y, bool rotate, bool scale);
    void action_clear_ref();

    void action_properties();
    void action_position_top();
    void action_position_left();
    void action_position_right();
    void action_position_bottom();
    void action_position_vertical();
    void action_position_horizontal();
    void action_align_top();
    void action_align_bottom();
    void action_align_left();
    void action_align_right();
    void action_align_all();
    void action_norm_scale();
    void action_clear_keys();

    void action_toggle_compare();
    void action_toggle_compare_wink();
    bool compare_enabled();

    void resize_controls();
 private:
    float fix_angle(float angle);
    void clear(bool clear_controls = true);
    //void load(Project* project, unsigned int index);
    bool wait_video_open();
    void open_video();
    void update_clipping_list();
    void update_seek_bar();
    void goto_selected_clipping_key();
    void frame_sleep(double fps);
    void display_speed();
    void redraw_frame(bool update_key_list=false);
    void load_images();
    void set_widget_image(Fl_Widget* widget, std::shared_ptr<Fl_Image> image);
    void update_title();
 private:
    static void button_callback(Fl_Widget* widget, void *userdata);
    static void video_list_callback(Fl_Widget* widget, void *userdata);
    static void seek_bar_callback(Fl_Widget* widget, void *userdata);
    void update_buffers(bool frame_changed);
    void double_click(void *component);
    void key_list_auto_selection();
    void action_speed();
    void action_play();
    void action_pause();
    void action_next();
    void action_stop();
    void action_prior();
    void action_search();
    void action_delete();
    void action_cutoff1();
    void action_cutoff12();
    void action_cutoff2();
    void action_insert();
    void action_play_interval();
 private:
    std::string video_path_;
    Fl_Group *parent_;
    Fl_Group *window_;
    Fl_Group *components_group_;
    Fl_Group *buttons_group_;
    Fl_Box *frame_input_;
    Fl_Box *frame_counter_;
    Fl_Box *frame_time_;
    Fl_Box *video_duration_;
    Fl_Hor_Slider *seek_bar_;
    Fl_Button *btn_speed_;
    Fl_Button *btn_play_;
    Fl_Button *btn_pause_;
    Fl_Button *btn_stop_;
    Fl_Button *btn_next_;
    Fl_Button *btn_prior_;
    Fl_Button *btn_cutoff1_;
    Fl_Button *btn_cutoff12_;
    Fl_Button *btn_cutoff2_;
    Fl_Button *btn_search_;

    Fl_Button *btn_new_key_;
    Fl_Button *btn_del_key_;
    Fl_Button *btn_play_interval_;
    Fl_Select_Browser *key_list_;

    std::shared_ptr<PlayerWrapper> player_;
    ClippingEditor *clipping_editor_;

    MiniatureViewer *viewer_;
    std::unique_ptr<ClippingKeeper> keeper_;
 private:
    std::set<std::shared_ptr<Fl_Image> > images_;
    bool has_key_copy_;
    clipping_key_t key_copy_;
 private:
    unsigned int wink_lap_;
    unsigned int selected_clip_;
    bool wink_comparison_;
    bool video_opened_;
    bool open_failure_;
    bool in_key_list_;
    bool in_seek_bar_callback_;
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_WND_CUTTER_CUTTER_WINDOW_H_

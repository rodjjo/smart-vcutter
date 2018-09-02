/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_CUTTER_CUTTER_WINDOW_H_
#define SRC_WND_CUTTER_CUTTER_WINDOW_H_

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

#include "src/data/history.h"
#include "src/wnd_cutter/clipping_actions.h"
#include "src/wrappers/video_player.h"
#include "src/clippings/clipping_session.h"
#include "src/viewer/miniature_viewer.h"
#include "src/viewer/editor/clipping_editor.h"


namespace vcutter {

class CutterWindow : public ClippingActionsHandler {
 public:
    CutterWindow(Fl_Group *parent);
    virtual ~CutterWindow();

    ClippingActions *clipping_actions();
    Clipping *clipping();
    PlayerWrapper *player();

    void close();
    std::shared_ptr<ClippingRender> to_clipping();
    void poll_actions();
    bool visible();
    bool modified();
    uint64_t modified_version();
    void clear_modified();

    void pause();

    void cancel_operations();

    std::string get_video_path();
    void action_goto_reference();
    void action_create_ref();
    void action_use_ref(bool positionate_x, bool positionate_y, bool rotate, bool scale);
    void action_clear_ref();
    void action_toggle_compare();
    void action_toggle_compare_wink();

    bool compare_enabled();
    bool compare_alternate();

    void resize_controls();
 private:
    void handle_clipping_resized() override;
    void handle_clipping_opened(bool opened) override;
    void handle_clipping_keys_changed() override;
    void handle_buffer_modified() override;
    bool player_bar_active() override;

 private:
    void clear(bool clear_controls = true);
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
    void action_stop();
    void action_search();
    void action_delete();

    void action_play_interval();
 private:
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
    ClippingEditor *clipping_editor_;

    std::unique_ptr<ClippingActions> player_bar_;

    MiniatureViewer *viewer_;
 private:
    std::set<std::shared_ptr<Fl_Image> > images_;
 private:
    uint64_t clipping_version_;
    unsigned int wink_lap_;
    unsigned int selected_clip_;
    bool wink_comparison_;
    bool open_failure_;
    bool in_key_list_;
    bool in_seek_bar_callback_;
};

}  // namespace vcutter

#endif  // SRC_WND_CUTTER_CUTTER_WINDOW_H_

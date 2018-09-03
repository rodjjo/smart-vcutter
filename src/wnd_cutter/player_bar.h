/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_CUTTER_PLAYER_BAR_H_
#define SRC_WND_CUTTER_PLAYER_BAR_H_

#include <memory>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Image.H>

#include "src/wnd_cutter/clipping_actions.h"
#include "src/controls/button.h"

namespace vcutter {

class PlayerBar {
 public:
    PlayerBar(ClippingActions *actions, Fl_Group *parent);
    virtual ~PlayerBar();
    PlayerWrapper *player();
    void resize_controls();
    void update();
    int h();

 private:
    callback_t action_speed();
    static void seek_bar_callback(Fl_Widget* widget, void *userdata);
    void display_speed();

 private:
    bool in_seek_bar_callback_;
    std::set<std::shared_ptr<Fl_Image> > images_;
    ClippingActions *actions_;
    Fl_Group *parent_;
    Fl_Group *group_;
    Fl_Box *frame_input_;
    Fl_Box *frame_counter_;
    Fl_Box *frame_time_;
    Fl_Box *video_duration_;
    Fl_Hor_Slider *seek_bar_;
    std::unique_ptr<Button> btn_speed_;
    std::unique_ptr<Button> btn_play_;
    std::unique_ptr<Button> btn_pause_;
    std::unique_ptr<Button> btn_stop_;
    std::unique_ptr<Button> btn_next_;
    std::unique_ptr<Button> btn_prior_;
    std::unique_ptr<Button> btn_cutoff1_;
    std::unique_ptr<Button> btn_cutoff12_;
    std::unique_ptr<Button> btn_cutoff2_;
    std::unique_ptr<Button> btn_search_;
};

}  // namespace vcutter

#endif // SRC_WND_CUTTER_PLAYER_BAR_H_

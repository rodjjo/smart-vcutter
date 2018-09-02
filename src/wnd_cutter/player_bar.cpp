/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <FL/Fl.H>
#include "src/data/xpm.h"
#include "src/common/utils.h"
#include "src/wnd_cutter/player_bar.h"


namespace vcutter {

PlayerBar::PlayerBar(ClippingActions *actions, Fl_Group *parent) {
    in_seek_bar_callback_ = false;
    actions_ = actions;
    parent_ = parent;

    group_ = new Fl_Group(0,0, parent->w(), 30);
    group_->box(FL_UP_BOX);

    auto null_action = [](){};

    btn_speed_.reset(new Button("1.0", action_speed()));
    btn_play_.reset(new Button(xpm::image(xpm::button_play), actions->action_play()));
    btn_pause_.reset(new Button(xpm::image(xpm::button_pause), actions->action_pause()));
    btn_stop_.reset(new Button(xpm::image(xpm::button_stop), actions->action_stop()));
    btn_search_.reset(new Button(xpm::image(xpm::button_seek), actions->action_search()));
    btn_prior_.reset(new Button(xpm::image(xpm::button_prior), actions_->action_prior()));
    btn_next_.reset(new Button(xpm::image(xpm::button_next), actions_->action_next()));
    btn_cutoff1_.reset(new Button(xpm::image(xpm::button_begin),  actions_->action_cutoff1()));
    btn_cutoff12_.reset(new Button(xpm::image(xpm::button_scissor), actions_->action_cutoff12()));
    btn_cutoff2_.reset(new Button(xpm::image(xpm::button_end), actions_->action_cutoff2()));

    frame_input_ = new Fl_Box(FL_DOWN_BOX, btn_cutoff2_->x() + 28, 3, 75, 25, "0");
    frame_counter_ = new Fl_Box(FL_DOWN_BOX, frame_input_->x() + frame_input_->w() + 3, 3, 75, 25, "-");
    frame_time_ = new Fl_Box(FL_DOWN_BOX, frame_counter_->x() + frame_counter_->w() + 3, 3, 75, 25, "-");
    video_duration_ = new Fl_Box(FL_DOWN_BOX, frame_time_->x() + frame_time_->w() + 3, 3, 75, 25, "-");

    frame_input_->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);
    frame_counter_->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);

    frame_time_->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);
    video_duration_->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);

    int seek_bar_left = video_duration_->x() + video_duration_->w() + 3;
    seek_bar_ = new Fl_Hor_Slider(seek_bar_left, 3, parent->w() - seek_bar_left - 5, 25);
    seek_bar_->step(1);

    group_->end();

    btn_speed_->tooltip("Change the video's speed.");
    btn_play_->tooltip("Play the video.");
    btn_pause_->tooltip("Pause the video.");
    btn_stop_->tooltip("Stop and Restart the video.");
    btn_prior_->tooltip("[Left] Pause and go to previous frame.");
    btn_next_->tooltip("[Right] Pause and go to next frame.");
    btn_search_->tooltip("[Tab] Seek for video time.");
    btn_cutoff1_->tooltip("[F2] Set the begin of the animation.");
    btn_cutoff12_->tooltip("[F3] Set the begin and end of the animation.");
    btn_cutoff2_->tooltip("[F4] Set the end of the animation.");


    btn_search_->shortcut(FL_Tab);
    btn_cutoff1_->shortcut(FL_F + 2);
    btn_cutoff12_->shortcut(FL_F + 3);
    btn_cutoff2_->shortcut(FL_F + 4);

    resize_controls();
}

PlayerBar::~PlayerBar() {
}

PlayerWrapper *PlayerBar::player() {
    return actions_->player();
}

void PlayerBar::resize_controls() {
    int parent_x = parent_->x();
    int parent_y = parent_->y();

    group_->position(0, parent_->h() - 30);
    group_->size(parent_->w(), 30);

    btn_speed_->size(35, 25);
    btn_play_->size(25, 25);
    btn_pause_->size(25, 25);
    btn_stop_->size(25, 25);
    btn_prior_->size(25, 25);
    btn_next_->size(25, 25);
    btn_search_->size(25, 25);
    btn_cutoff1_->size(25, 25);
    btn_cutoff12_->size(25, 25);
    btn_cutoff2_->size(25, 25);
    frame_input_->size(75, 25);
    frame_counter_->size(75, 25);
    frame_time_->size(75, 25);
    video_duration_->size(75, 25);

    int position = 38;
    btn_speed_->position(0, btn_play_->y());
    btn_play_->position(position, btn_play_->y());
    btn_pause_->position(position += 27, btn_play_->y());
    btn_stop_->position(position += 27, btn_play_->y());
    btn_prior_->position(position += 27, btn_play_->y());
    btn_next_->position(position += 27, btn_play_->y());
    btn_search_->position(position += 27, btn_play_->y());
    btn_cutoff1_->position(position += 27, btn_play_->y());
    btn_cutoff12_->position(position += 27, btn_play_->y());
    btn_cutoff2_->position(position += 27, btn_play_->y());
    frame_input_->position(position += 27, btn_play_->y());
    frame_counter_->position(position += 77, btn_play_->y());
    frame_time_->position(position += 77, btn_play_->y());
    video_duration_->position(position += 77, btn_play_->y());

    seek_bar_->position(position += 77,  btn_play_->y());
    seek_bar_->size(parent_->w() - seek_bar_->x() - 5, 25);

    seek_bar_->callback(seek_bar_callback, this);

    parent_->position(parent_x, parent_y);
}


void PlayerBar::display_speed() {
    if (actions_->clipping()) {
        char temp[50];
        snprintf(temp, sizeof(temp), "%0.2f", player()->get_speed());
        btn_speed_->change_label(temp);
    }
}

callback_t PlayerBar::action_speed() {
    return [this] () {
        player()->change_speed(!Fl::event_shift());
        display_speed();
    };
}

int PlayerBar::h() {
    return group_->h();
}

void PlayerBar::seek_bar_callback(Fl_Widget* widget, void *userdata) {
    auto bar = static_cast<PlayerBar *>(userdata);
    if (bar->in_seek_bar_callback_) {
        return;
    }

    if (bar->actions_->clipping()) {
        bar->actions_->player()->seek_frame(bar->seek_bar_->value());
        bar->actions_->handler()->handle_buffer_modified();
    }
}

void PlayerBar::update() {
    if (!actions_->clipping()) {
        frame_input_->label("0");
        frame_counter_->label("0");
        return;
    }

    in_seek_bar_callback_ = true;
    if (seek_bar_->maximum() != player()->info()->count()) {
        seek_bar_->maximum(player()->info()->count());
    }
    if (seek_bar_->value() !=  player()->info()->position()) {
        seek_bar_->value(player()->info()->position());
    }
    char temp[55] = "";
    snprintf(temp, sizeof(temp), "%u", player()->info()->position());
    frame_input_->copy_label(temp);
    snprintf(temp, sizeof(temp), "%u", player()->info()->count());
    frame_counter_->copy_label(temp);

    seconds_to_str(temp, sizeof(temp), player()->info()->time());
    frame_time_->copy_label(temp);
    seconds_to_str(temp, sizeof(temp), player()->info()->duration());
    video_duration_->copy_label(temp);

    in_seek_bar_callback_ = false;
}

}  // namespace vcutter

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cmath>
#include <GL/gl.h>

#include "src/common/utils.h"
#include "src/data/xpm.h"
#include "src/wnd_common/common_dialogs.h"

#include "src/wnd_cutter/cutter_window.h"


namespace vcutter {

#define CLIPPING_LIST_WIDTH 320


CutterWindow::CutterWindow(Fl_Group *parent) {
    parent_ = parent;
    open_failure_ = false;
    in_key_list_ = false;
    wink_comparison_ = false;
    in_seek_bar_callback_ = false;
    clipping_version_ = 0;
    selected_clip_ = 0;
    wink_lap_ = 0;
    int parent_x = parent->x();
    int parent_y = parent->y();

    parent->position(0, 0);
    parent->begin();
    window_ = new Fl_Group(0, 0, parent->w(), parent->h());
    window_->begin();

    components_group_ = new Fl_Group(0,0, window_->w(), window_->h() - 30);
    components_group_->box(FL_DOWN_BOX);
    clipping_editor_  = new ClippingEditor(5, 5, window_->w() - CLIPPING_LIST_WIDTH - 10,  window_->h() - 45);

    btn_new_key_ = new Fl_Button(clipping_editor_->w() + 7, 3, 25, 25, "");
    btn_del_key_ = new Fl_Button(btn_new_key_->x() + 27, 3, 25, 25, "");
    btn_play_interval_ = new Fl_Button(btn_del_key_->x() + 27, 3, 25, 25, "");

    key_list_ = new Fl_Select_Browser(btn_new_key_->x(), 30, CLIPPING_LIST_WIDTH, window_->h() * 30);

    viewer_ = new MiniatureViewer(btn_new_key_->x(), key_list_->y() + key_list_->h(), key_list_->w(), key_list_->w());

    components_group_->end();

    player_bar_.reset(new ClippingActions(this));

    buttons_group_ = new Fl_Group(0,0, window_->w(), 30);
    buttons_group_->box(FL_UP_BOX);

    btn_speed_ = new Fl_Button(5, 3, 35, 25, "1.0");
    btn_play_ = new Fl_Button(btn_speed_->x() + 38, 3, 25, 25, "");
    btn_pause_ = new Fl_Button(btn_play_->x() + 27, 3, 25, 25, "");
    btn_stop_ = new Fl_Button(btn_pause_->x() + 27, 3, 25, 25, "");
    btn_prior_ = new Fl_Button(btn_stop_->x() + 27, 3, 25, 25, "");
    btn_next_ = new Fl_Button(btn_prior_->x() + 27, 3, 25, 25, "");
    btn_search_ = new Fl_Button(btn_next_->x() + 27, 3, 25, 25, "");
    btn_cutoff1_ = new Fl_Button(btn_search_->x() + 27, 3, 25, 25, "");
    btn_cutoff12_ = new Fl_Button(btn_cutoff1_->x() + 27, 3, 25, 25, "");
    btn_cutoff2_ = new Fl_Button(btn_cutoff12_->x() + 27, 3, 25, 25, "");
    frame_input_ = new Fl_Box(FL_DOWN_BOX, btn_cutoff2_->x() + 28, 3, 75, 25, "0");
    frame_counter_ = new Fl_Box(FL_DOWN_BOX, frame_input_->x() + frame_input_->w() + 3, 3, 75, 25, "-");
    frame_time_ = new Fl_Box(FL_DOWN_BOX, frame_counter_->x() + frame_counter_->w() + 3, 3, 75, 25, "-");
    video_duration_ = new Fl_Box(FL_DOWN_BOX, frame_time_->x() + frame_time_->w() + 3, 3, 75, 25, "-");

    frame_input_->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);
    frame_counter_->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);

    frame_time_->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);
    video_duration_->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);

    int seek_bar_left = video_duration_->x() + video_duration_->w() + 3;
    seek_bar_ = new Fl_Hor_Slider(seek_bar_left, 3, window_->w() - seek_bar_left - 5, 25);
    seek_bar_->step(1);

    buttons_group_->end();
    buttons_group_->position(0, window_->h() - 30);

    btn_speed_->callback(button_callback, this);
    btn_play_->callback(button_callback, this);
    btn_pause_->callback(button_callback, this);
    btn_stop_->callback(button_callback, this);
    btn_prior_ ->callback(button_callback, this);
    btn_next_->callback(button_callback, this);
    btn_search_->callback(button_callback, this);
    btn_cutoff1_->callback(button_callback, this);
    btn_cutoff12_->callback(button_callback, this);
    btn_cutoff2_->callback(button_callback, this);
    btn_new_key_->callback(button_callback, this);
    btn_del_key_->callback(button_callback, this);
    btn_play_interval_->callback(button_callback, this);

    seek_bar_->clear_visible_focus();
    btn_speed_->clear_visible_focus();
    btn_play_->clear_visible_focus();
    btn_pause_->clear_visible_focus();
    btn_stop_->clear_visible_focus();
    btn_prior_->clear_visible_focus();
    btn_next_->clear_visible_focus();
    btn_search_->clear_visible_focus();
    btn_cutoff1_->clear_visible_focus();
    btn_cutoff12_->clear_visible_focus();
    btn_cutoff2_->clear_visible_focus();
    btn_new_key_->clear_visible_focus();
    btn_del_key_->clear_visible_focus();
    btn_play_interval_->clear_visible_focus();

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
    btn_new_key_->tooltip("[Insert] Insert a mark at current frame.");
    btn_del_key_->tooltip("[Delete] Remove the current frame mark.");
    btn_play_interval_->tooltip("Play the animation interval.");
    seek_bar_->callback(seek_bar_callback, this);

    window_->end();
    parent->end();

    key_list_->callback(video_list_callback, this);

    viewer_->cursor(FL_CURSOR_HAND);

    btn_del_key_->shortcut(FL_Delete);
    btn_new_key_->shortcut(FL_Insert);
    btn_search_->shortcut(FL_Tab);
    btn_cutoff1_->shortcut(FL_F + 2);
    btn_cutoff12_->shortcut(FL_F + 3);
    btn_cutoff2_->shortcut(FL_F + 4);

    parent->position(parent_x, parent_y);

    window_->hide();

    load_images();
    update_title();
}

CutterWindow::~CutterWindow() {
    clear();
}

ClippingActions *CutterWindow::clipping_actions() {
    return player_bar_.get();
}

Clipping* CutterWindow::clipping() {
    return player_bar_->clipping();
}

PlayerWrapper *CutterWindow::player() {
    if (clipping()) {
        return player();
    }
    return NULL;
}

void CutterWindow::set_widget_image(Fl_Widget* widget, std::shared_ptr<Fl_Image> image) {
    images_.insert(image);
    widget->image(image.get());
    widget->align(FL_ALIGN_IMAGE_BACKDROP);
}

void CutterWindow::load_images() {
    set_widget_image(btn_play_, xpm::image(xpm::button_play));
    set_widget_image(btn_cutoff1_, xpm::image(xpm::button_begin));
    set_widget_image(btn_cutoff2_, xpm::image(xpm::button_end));
    set_widget_image(btn_next_, xpm::image(xpm::button_next));
    set_widget_image(btn_pause_, xpm::image(xpm::button_pause));
    set_widget_image(btn_prior_, xpm::image(xpm::button_prior));
    set_widget_image(btn_cutoff12_, xpm::image(xpm::button_scissor));
    set_widget_image(btn_stop_, xpm::image(xpm::button_stop));
    set_widget_image(btn_search_, xpm::image(xpm::button_seek));
    set_widget_image(btn_new_key_, xpm::image(xpm::button_add));
    set_widget_image(btn_del_key_, xpm::image(xpm::button_delete));
    set_widget_image(btn_play_interval_, xpm::image(xpm::button_play));
}

void CutterWindow::update_title() {
    if (!visible() || !clipping()) {
        parent_->window()->label("Smart Loop Creator");
        return;
    }
    char title[1024] = "";
    snprintf(title, sizeof(title), "Smart Loop Creator (%dx%d)->(%dx%d) %s",
        player()->info()->w(), player()->info()->h(),
        clipping()->w(), clipping()->h(),
        clipping()->video_path().c_str());
    parent_->window()->copy_label(title);
}

void CutterWindow::resize_controls() {
    int parent_x = parent_->x();
    int parent_y = parent_->y();

    parent_->position(0, 0);
    window_->position(0, 0);
    window_->size(parent_->w(), parent_->h());

    buttons_group_->position(0, parent_->h() - 30);
    buttons_group_->size(parent_->w(), 30);

    clipping_editor_->size(parent_->w() - CLIPPING_LIST_WIDTH - 10,  parent_->h() - buttons_group_->h() - parent_->y() - 10);
    clipping_editor_->position(5, 5);
    clipping_editor_->invalidate();

    btn_new_key_->position(parent_->w() - CLIPPING_LIST_WIDTH, 3);
    btn_del_key_->position(btn_new_key_->x() + 27, 3);
    btn_play_interval_->position(btn_del_key_->x() + 27, 3);

    static_cast<Fl_Widget *>(key_list_)->position(btn_new_key_->x(), 33);
    int key_list_w = parent_->w() - btn_new_key_->x();
    key_list_->size(key_list_w, clipping_editor_->h() - key_list_w - buttons_group_->h() - 38);

    viewer_->position(btn_new_key_->x(), key_list_->y() + key_list_->h());
    viewer_->size(key_list_->w(), key_list_->w());

    btn_new_key_->size(25, 25);
    btn_del_key_->size(25, 25);
    btn_play_interval_->size(25, 25);

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

    parent_->position(parent_x, parent_y);
    if (visible()) {
        update_buffers(true);
        redraw_frame();
    }
}

void CutterWindow::clear(bool clear_controls) {
    player_bar_->close();
    open_failure_ = false;
    wink_lap_ = 0;
    clipping_version_ = 0;
    wink_comparison_ = false;
    viewer_->invalidate();
    clipping_editor_->invalidate();
    if (clear_controls) {
        key_list_->clear();
        frame_input_->label("0");
        frame_counter_->label("0");
        window_->hide();
        display_speed();
    }
    update_title();
}

std::string CutterWindow::get_video_path() {
    if (clipping()) {
        return clipping()->video_path();
    }
    return "";
}

bool CutterWindow::modified() {

    return visible() && clipping_version_ != clipping()->version();
}

uint64_t CutterWindow::modified_version() {
    if (!modified()) {
        return 0;
    }
    return clipping()->version();
}

void CutterWindow::clear_modified() {
    clipping_version_ = clipping()->version();
}

void CutterWindow::handle_clipping_keys_changed() {
    redraw_frame(true);
}

void CutterWindow::handle_clipping_opened(bool opened) {
    if (!opened) {
        clear(true);
        return;
    }

    player()->seek_frame(clipping()->first_frame());

    update_seek_bar();
    redraw_frame();

    window_->show();
    update_title();
    clipping_version_ = clipping()->version();

    update_buffers(true);
}

void CutterWindow::close() {
    clear(true);
}


bool CutterWindow::visible() {
    return window_->visible();
}

bool CutterWindow::player_bar_active() {
    return visible();
}

std::shared_ptr<ClippingRender> CutterWindow::to_clipping() {
    return clipping()->clone();
}

void CutterWindow::button_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<CutterWindow *>(userdata);
    if (widget == window->btn_speed_) {
        window->action_speed();
    } else if (widget == window->btn_play_) {
        window->action_play();
    } else if (widget == window->btn_pause_) {
        window->action_pause();
    } else if (widget == window->btn_stop_) {
        window->action_stop();
    } else if (widget == window->btn_prior_) {
        window->clipping_actions()->action_prior()();
    } else if (widget == window->btn_next_) {
        window->clipping_actions()->action_next()();
    } else if (widget == window->btn_search_) {
        window->action_search();
    } else if (widget == window->btn_del_key_) {
        window->clipping_actions()->action_delete()();
    } else if (widget == window->btn_new_key_) {
        window->clipping_actions()->action_insert()();
    } else if (widget == window->btn_cutoff1_) {
        window->clipping_actions()->action_cutoff1()();
    } else if (widget == window->btn_cutoff12_) {
        window->clipping_actions()->action_cutoff12()();
    } else if (widget == window->btn_cutoff2_) {
        window->clipping_actions()->action_cutoff2()();
    } else if (widget == window->btn_play_interval_) {
        window->action_play_interval();
    }
}

void CutterWindow::goto_selected_clipping_key() {
    if (!key_list_->value()) {
        return;
    }

    uint32_t index = key_list_->value() - 1;
    if (index < clipping()->keys().size()) {
        auto frame = clipping()->at_index(index).frame;

        player()->pause();
        player()->seek_frame(frame);
        update_buffers(true);
    }
}

void CutterWindow::video_list_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<CutterWindow *>(userdata);
    if (window->in_key_list_) {
        return;
    }
    window->in_key_list_ = true;
    auto selected_index = window->key_list_->value();
    if (selected_index) {
        window->key_list_->deselect();
        window->key_list_->select(selected_index);
        window->goto_selected_clipping_key();
    }
    window->in_key_list_ = false;
}

void CutterWindow::action_play() {
    player()->play();
}

void CutterWindow::action_pause() {
    player()->pause();
}

void CutterWindow::action_stop() {
    player()->stop();
}

void CutterWindow::handle_clipping_resized() {
    redraw_frame();
    update_title();
}

void CutterWindow::action_search() {
    if (!clipping()) {
        return;
    }

    const char *timestr = ask_value("Time in format hh:mm:ss or mm:ss or number of seconds");
    if (!timestr) {
        return;
    }

    int hours, minutes, seconds, frame;

    if (sscanf(timestr, "%d:%d:%d", &hours, &minutes, &seconds) == 3) {
        seconds = hours * 36000 + minutes * 60 + seconds;
    } else if (sscanf(timestr, "%d:%d", &minutes, &seconds) == 2) {
        seconds = minutes * 60 + seconds;
    } else if (*timestr == 'f' && sscanf(timestr, "f%d", &frame) == 1) {
        if (frame > 0) {
            frame -= 1;
        }
        player()->seek_frame(frame + 1);
        update_buffers(true);
        return;
    } else if (sscanf(timestr, "%d", &seconds) != 1) {
        return;
    }

    player()->seek_time((seconds + 1) * 1000);
    update_buffers(true);
}

void CutterWindow::action_play_interval() {
    if (!clipping()) {
        return;
    }

    if (clipping()->keys().empty()) {
        return;
    }

    player()->play(clipping()->first_frame(), clipping()->last_frame());
}

void CutterWindow::pause() {
    if (visible() && clipping()) {
        player()->pause();
    }
}


void CutterWindow::action_use_ref(bool positionate_x, bool positionate_y, bool rotate, bool scale) {
    if (!visible()) {
        return;
    }
    if (player()->is_playing()) {
        show_error("It's necessary to pause the video.");
        return;
    }
    if (!clipping()->ref().has_ref()) {
        show_error("It's necessary to create references before use it.");
        return;
    }
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "x_enabled", positionate_x);
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "y_enabled", positionate_y);
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "scale_enabled", scale);
    clipping_editor_->operation_option(MAGIC_USE_OPERATION_NAME, "rotate_enabled", rotate);
    clipping_editor_->activate_operation(MAGIC_USE_OPERATION_NAME);
}

void CutterWindow::action_create_ref() {
    if (!visible()) {
        return;
    }
    if (player()->is_playing()) {
        show_error("It's necessary to pause the video.");
        return;
    }
    clipping()->ref().clear_reference();
    clipping_editor_->activate_operation(MAGIC_DEFINE_OPERATION_NAME);
}

void CutterWindow::action_clear_ref() {
    if (!visible()) {
        return;
    }
    clipping()->ref().clear_reference();
    update_buffers(true);
}

void CutterWindow::action_goto_reference() {
    uint32_t frame = 0;
    if (clipping()->ref().get_reference_frame(&frame)) {
        player()->pause();
        player()->seek_frame(frame);
        update_buffers(true);
    }
}

void CutterWindow::double_click(void *component) {
    if (component == viewer_) {
        clipping_actions()->action_properties()();
    }
}

void CutterWindow::update_clipping_list() {
    in_key_list_ = true;
    char temp_buffer[100] = "";
    int selection = key_list_->value();
    uint32_t size = key_list_->size();

    key_list_->clear();
    uint32_t ref_frame = -1;
    clipping()->ref().get_reference_frame(&ref_frame);

    const ClippingKey *previous = NULL;
    for (const auto & k : clipping()->keys()) {
        key_list_->add((k.description(previous) + (ref_frame == k.frame ? " <r" : "")).c_str());
        previous = &k;
    }

    if (size == static_cast<uint32_t>(key_list_->size()) && selection != 0) {
        key_list_->deselect();
        key_list_->value(selection);
        key_list_->select(selection);
    }
    in_key_list_ = false;
}

void CutterWindow::seek_bar_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<CutterWindow *>(userdata);
    if (window->in_seek_bar_callback_) {
        return;
    }

    if (window->clipping()) {
        window->player()->seek_frame(window->seek_bar_->value());
        window->update_buffers(true);
    }
}

void CutterWindow::redraw_frame(bool update_key_list) {
    if (update_key_list) {
        update_clipping_list();
        viewer_->update_preview(clipping());
    }

    clipping_editor_->redraw();
    viewer_->redraw();
    update_seek_bar();
}

void CutterWindow::update_seek_bar() {
    if (!clipping()) {
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

void CutterWindow::key_list_auto_selection() {
    in_key_list_ = true;
    selected_clip_ = clipping()->find_index(player()->info()->position()) + 1;
    if (selected_clip_ > 0 && selected_clip_ <= static_cast<uint32_t>(key_list_->size())) {
        key_list_->value(selected_clip_);
    }
    in_key_list_ = false;
}

void CutterWindow::handle_buffer_modified() {
    update_buffers(true);
}

void CutterWindow::update_buffers(bool frame_changed) {
    if (player()->info()->error()) {
        return;
    }

    if (frame_changed) {
        update_seek_bar();
        redraw_frame();
        key_list_auto_selection();
        clipping_editor_->update(clipping());
        viewer_->update_preview(clipping());
    } else {
        clipping_editor_->draw_operations();
        if (clipping_editor_->key_changed(true)) {
            update_clipping_list();
            viewer_->update_preview(clipping());
        }
    }

    if (clipping()->w() < 15 || clipping()->h() < 15) {
        clipping()->wh(player()->info()->w(), player()->info()->h());
    }

    if (clipping()->keys().size() != static_cast<uint32_t>(key_list_->size())) {
        update_clipping_list();
    }
}

void CutterWindow::poll_actions() {
    if (clipping()) {
        update_buffers(player()->frame_changed(true));
        if (wink_comparison_ && clipping_editor_->compare_box() && !player()->is_playing()) {
            ++wink_lap_;
            if (wink_lap_ > 11) {
                wink_lap_ = 0;
                clipping_editor_->wink_compare_box();
            }
        }
    }
}

void CutterWindow::display_speed() {
    if (clipping()) {
        char temp[50];
        snprintf(temp, sizeof(temp), "%0.2f", player()->get_speed());
        btn_speed_->copy_label(temp);
    }
}

void CutterWindow::action_speed() {
    player()->change_speed(!Fl::event_shift());
    display_speed();
}

void CutterWindow::action_toggle_compare() {
    clipping_editor_->toggle_compare_box();
}

void CutterWindow::action_toggle_compare_wink() {
    if (!visible()) {
        return;
    }
    wink_comparison_ = wink_comparison_ == false;
}

bool CutterWindow::compare_alternate() {
    return wink_comparison_;
}

bool CutterWindow::compare_enabled() {
    return clipping_editor_->compare_box();
}

void CutterWindow::cancel_operations() {
    if (visible()) {
        clipping_editor_->cancel_operations();
    }
}


}  // namespace vcutter

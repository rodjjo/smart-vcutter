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
    has_key_copy_ = false;

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
    if (!visible() || !clipping_) {
        parent_->window()->label("Smart Loop Creator");
        return;
    }
    char title[1024] = "";
    snprintf(title, sizeof(title), "Smart Loop Creator (%dx%d)->(%dx%d) %s",
        clipping_->player()->info()->w(), clipping_->player()->info()->h(),
        clipping_->w(), clipping_->h(),
        clipping_->video_path().c_str());
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

void CutterWindow::prior() {
    if (!visible()) {
        return;
    }
    action_prior();
}

void CutterWindow::next() {
    if (!visible()) {
        return;
    }
    action_next();
}


void CutterWindow::clear(bool clear_controls) {
    clipping_.reset();
    open_failure_ = false;
    has_key_copy_ = false;
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
    if (clipping_) {
        return clipping_->video_path();
    }
    return "";
}

bool CutterWindow::modified() {

    return visible() && clipping_version_ != clipping_->version();
}

uint64_t CutterWindow::modified_version() {
    if (!modified()) {
        return 0;
    }
    return clipping_->version();
}

void CutterWindow::clear_modified() {
    clipping_version_ = clipping_->version();
}

bool CutterWindow::open_video(const std::string& video_path) {
    clear();

    clipping_.reset(new ClippingSession("cwnd", video_path.c_str(), true));
    return handle_opened_clipping();
}

bool CutterWindow::handle_opened_clipping() {
    if (!clipping_->good()) {
        open_failure_ = true;
        clear(true);
        show_error("Não foi possivel abrir o arquivo de video.");
        return false;
    }

    clipping_->player()->seek_frame(clipping_->first_frame());

    update_seek_bar();
    redraw_frame();

    window_->show();
    update_title();
    clipping_version_ = clipping_->version();

    update_buffers(true);

    return true;
}

bool CutterWindow::restore_session() {
    std::shared_ptr<ClippingSession> restored(std::move(ClippingSession::restore_session("cwnd")));

    if (!restored) {
        return false;
    }

    clipping_.swap(restored);

    return handle_opened_clipping();
}

bool CutterWindow::open_clipping(const std::string& path) {
    clear();
    clipping_.reset(new ClippingSession("cwnd", path.c_str(), false));
    return handle_opened_clipping();
}


bool CutterWindow::save(History *history) {
    if (!visible() || !clipping_) {
        return true;
    }

    if (clipping_->saved_path().empty()) {
        return save_as(history);
    }

    clipping_->save(clipping_->saved_path().c_str());
    return true;
}

bool CutterWindow::save_as(History *history) {
    if (!visible() || !clipping_) {
        return true;
    }

    const char *key = "main-window-project-dir";
    std::string directory = (*history)[key];
    std::string path = output_prj_file_chooser(&directory);

    if (!directory.empty()) {
        history->set(key, directory.c_str());
    }

    if (path.empty()) {
        return false;
    }

    if (filepath_exists(path.c_str()) && !ask("The file already exists. Overwrite it ?")) {
        return false;
    }

    clipping_->save(path.c_str());
    return true;
}


void CutterWindow::close() {
    clear(true);
}


bool CutterWindow::visible() {
    return window_->visible();
}

void CutterWindow::open_video() {

}

std::shared_ptr<Clipping> CutterWindow::to_clipping() {
    return clipping_;
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
        window->action_prior();
    } else if (widget == window->btn_next_) {
        window->action_next();
    } else if (widget == window->btn_search_) {
        window->action_search();
    } else if (widget == window->btn_del_key_) {
        window->action_delete();
    } else if (widget == window->btn_new_key_) {
        window->action_insert();
    } else if (widget == window->btn_cutoff1_) {
        window->action_cutoff1();
    } else if (widget == window->btn_cutoff12_) {
        window->action_cutoff12();
    } else if (widget == window->btn_cutoff2_) {
        window->action_cutoff2();
    } else if (widget == window->btn_play_interval_) {
        window->action_play_interval();
    }
}

void CutterWindow::goto_selected_clipping_key() {
    if (!key_list_->value()) {
        return;
    }

    uint32_t index = key_list_->value() - 1;
    if (index < clipping_->keys().size()) {
        auto frame = clipping_->at_index(index).frame;

        clipping_->player()->pause();
        clipping_->player()->seek_frame(frame);
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

void CutterWindow::action_clear_keys() {
    if (clipping_->keys().empty()) {
        show_error("There is no keys to clear");
        return;
    }
    if (!ask("Are you sure to delete all keys ?")) {
        return;
    }
    clipping_->remove_all();
    update_clipping_list();
}

void CutterWindow::action_play() {
    clipping_->player()->play();
}

void CutterWindow::action_pause() {
    clipping_->player()->pause();
}

void CutterWindow::action_next() {
    if (!clipping_->player()->is_playing() && Fl::event_shift() && clipping_->player()->info()->position() + 33 < clipping_->player()->info()->count()) {
        clipping_->player()->seek_frame(clipping_->player()->info()->position() + 33);
        update_buffers(true);
        return;
    }
    clipping_->player()->next();
    update_buffers(true);
}

void CutterWindow::action_stop() {
    clipping_->player()->stop();
}

void CutterWindow::action_prior() {
    if (!clipping_->player()->is_playing() && Fl::event_shift() && clipping_->player()->info()->position() - 33 > 0) {
        clipping_->player()->seek_frame(clipping_->player()->info()->position() - 33);
        update_buffers(true);
        return;
    }

    clipping_->player()->prior();
    update_buffers(true);
}

void CutterWindow::action_properties() {
    if (!visible()) {
        return;
    }

    unsigned int w = clipping_->w();
    unsigned int h = clipping_->h();
    if (CutterOptionsWindow::edit_properties(clipping_->player()->info()->w(), clipping_->player()->info()->h(), &w, &h)) {
        clipping_->wh(w, h);
        redraw_frame();
        update_title();
    }
}

void CutterWindow::action_search() {
    if (!clipping_) {
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
        clipping_->player()->seek_frame(frame + 1);
        update_buffers(true);
        return;
    } else if (sscanf(timestr, "%d", &seconds) != 1) {
        return;
    }

    clipping_->player()->seek_time((seconds + 1) * 1000);
    update_buffers(true);
}

void CutterWindow::action_play_interval() {
    if (!clipping_) {
        return;
    }

    if (clipping_->keys().empty()) {
        return;
    }

    clipping_->player()->play(clipping_->first_frame(), clipping_->last_frame());
}

void CutterWindow::pause() {
    if (visible() && clipping_) {
        clipping_->player()->pause();
    }
}

bool CutterWindow::is_paused(bool show_message) {
    if (!clipping_) {
        return false;
    }

    if (clipping_->player()->is_playing()) {
        if (show_message) {
            show_error("This action requires that you pause the video before");
        }
        return false;
    }
    return true;
}

void CutterWindow::action_insert() {
    if (!is_paused(true)) {
        return;
    }

    auto key = clipping_->at(clipping_->player()->info()->position());

    if (key.computed()) {
        clipping_->add(key);
        update_clipping_list();
    } else {
        show_error("There already is a mark at this frame.");
    }
}

bool CutterWindow::has_copy() {
    return has_key_copy_;
}

bool CutterWindow::has_copy(const char* paused_message) {
    if (!has_copy()) {
        return false;
    }

    return is_paused(paused_message);
}

void CutterWindow::copy() {
    if (!is_paused(true)) {
        return;
    }

    key_copy_ = clipping_->at(clipping_->player()->info()->position());
    has_key_copy_ = true;
}

void CutterWindow::no_copy() {
    has_key_copy_ = false;
}

void CutterWindow::past() {
    if (!has_copy("It's necessary to pause the video before paste a mark.")) {
        return;
    }
    auto copy = key_copy_;
    copy.frame = clipping_->player()->info()->position();
    clipping_->add(copy);
    update_clipping_list();
    redraw_frame(true);
}

void CutterWindow::past_rotation() {
    if (!has_copy("It's necessary to pause the video before paste the mark's rotation.")) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.angle(key_copy_.angle());
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::past_scale() {
    if (!has_copy("It's necessary to pause the video before paste the mark's scale.")) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.scale = key_copy_.scale;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::past_position(bool x, bool y) {
    if (!has_copy("It's necessary to pause the video before paste the mark's position.")) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    if (x)
        key.px = key_copy_.px;
    if (y)
        key.py = key_copy_.py;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::clear_rotation() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.angle(0);
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::clear_scale() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.scale = 1;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::scale_half() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.scale = 0.5;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::scale_half_2() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.scale = 0.25;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::scale_2() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.scale = 2;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::scale_3() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.scale = 3;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::clear_position() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.px = clipping_->player()->info()->w() / 2;
    key.py = clipping_->player()->info()->h() / 2;
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::rotation_90() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.angle(90);
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::rotation_180() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.angle(180);
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::rotation_270() {
    if (!is_paused(true)) {
        return;
    }
    auto key = clipping_->at(clipping_->player()->info()->position());
    key.angle(270);
    clipping_->add(key);
    redraw_frame(true);
}

void CutterWindow::swap_wh() {
    if (!is_paused(true)) {
        return;
    }

    uint32_t video_w = clipping_->player()->info()->w();
    uint32_t video_h = clipping_->player()->info()->h();

    if (!video_h || !video_w) {
        show_error("Unxpected error width or height invalid in this video");
        return;
    }

    if (!ask("Are you sure to swap width/height ?\nThis may resize the canvas.\nThis will rotate +90 degrees")) {
        return;
    }

    uint32_t width = clipping_->w();
    uint32_t height = clipping_->h();
    double scale = 1.0;
    double scaled = 1.0;

    if (width > video_h) {
        scale  = video_h / static_cast<float>(width);
        width *= scale;
        height *= scale;
        scaled *= scale;
    }

    if (width > video_w) {
        scale  = video_w / static_cast<float>(width);
        width *= scale;
        height *= scale;
        scaled *= scale;
    }

    if (height > video_w) {
        scale  = video_w / static_cast<float>(height);
        width *= scale;
        height *= scale;
        scaled *= scale;
    }

    if (height > video_h) {
        scale  = video_h / static_cast<float>(height);
        width *= scale;
        height *= scale;
        scaled *= scale;
    }

    if (height % 2) {
        height -= 1;
    }

    clipping_->wh(height, width);

    for (auto k : clipping_->keys()) {
        k.angle(k.angle() + 90);
        k.scale *= (1.0 / scaled);
        clipping_->add(k);
    }

    redraw_frame(true);
}

void CutterWindow::rotate_all_180() {
    if (!is_paused(true)) {
        return;
    }

    if (!ask("Are you sure to rotate all keys 180 degrees ?")) {
        return;
    }

    for (auto k : clipping_->keys()) {
        k.angle(k.angle() + 180);
        clipping_->add(k);
    }

    redraw_frame(true);
}

void CutterWindow::scale_all() {
    if (!is_paused(true)) {
        return;
    }

    const char *scalestr = ask_value("A number to scale all keys (0.25 to 4.0): ");
    if (!scalestr) {
        return;
    }

    float scale = 1.0;
    if (sscanf(scalestr, "%f", &scale) != 1) {
        show_error("The value entered is not a number. The scale must be from 0.25 to 4.0");
        return;
    }

    if (scale < 0.25 || scale > 4.0) {
        show_error("The scale must be from 0.25 to 4.0");
        return;
    }

    for (auto k : clipping_->keys()) {
        k.scale *= scale;
        clipping_->add(k);
    }

    redraw_frame(true);
}

void CutterWindow::action_delete() {
    if (!clipping_) {
        return;
    }
    if (clipping_->player()->is_playing()) {
        show_error("It's necessary to pause the video before delete a mark.");
        return;
    }
    if (clipping_->keys().size() < 1) {
        show_error("There is no item to remove.");
        return;
    }

    auto key = clipping_->at(clipping_->player()->info()->position());

    if (key.computed()) {
        show_error("The current video's frame does not have a mark to remove.");
        return;
    }

    if (ask("Are you sure to delete current")) {
        clipping_->remove(key.frame);
        update_clipping_list();
    }
}

void CutterWindow::action_cutoff1() {
    if (!clipping_) {
        return;
    }
    if (clipping_->player()->is_playing()) {
        show_error("It's necessary to pause the video before cut.");
        return;
    }
    if (ask("This action will remove several marks from begin. Are you sure ?")) {
        clipping_->define_start(clipping_->player()->info()->position());
        update_clipping_list();
    }
}

void CutterWindow::action_cutoff12() {
    if (!clipping_) {
        return;
    }
    if (clipping_->player()->is_playing()) {
        show_error("It's necessary to pause the video before cut.");
        return;
    }
    if (ask("This action will remove several marks. Are you sure ?")) {
        clipping_->remove_all(clipping_->player()->info()->position());
        update_clipping_list();
    }
}

void CutterWindow::action_cutoff2() {
    if (!clipping_) {
        return;
    }

    if (clipping_->player()->is_playing()) {
        show_error("It's necessary to pause the video before cut.");
        return;
    }

    if (ask("This action will remove several marks to the end. Are you sure ?")) {
        clipping_->define_end(clipping_->player()->info()->position());
        update_clipping_list();
    }
}

void CutterWindow::action_use_ref(bool positionate_x, bool positionate_y, bool rotate, bool scale) {
    if (!visible()) {
        return;
    }
    if (clipping_->player()->is_playing()) {
        show_error("It's necessary to pause the video.");
        return;
    }
    if (!clipping_->ref().has_ref()) {
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
    if (clipping_->player()->is_playing()) {
        show_error("It's necessary to pause the video.");
        return;
    }
    clipping_->ref().clear_reference();
    clipping_editor_->activate_operation(MAGIC_DEFINE_OPERATION_NAME);
}

void CutterWindow::action_clear_ref() {
    if (!visible()) {
        return;
    }
    clipping_->ref().clear_reference();
    update_buffers(true);
}

void CutterWindow::action_goto_reference() {
    uint32_t frame = 0;
    if (clipping_->ref().get_reference_frame(&frame)) {
        clipping_->player()->pause();
        clipping_->player()->seek_frame(frame);
        update_buffers(true);
    }
}

void CutterWindow::action_position_top() {
     if (!is_paused(true)) {
        return;
    }
    clipping_->positionate_top(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_position_left() {
     if (!is_paused(true)) {
        return;
    }
    clipping_->positionate_left(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_position_right() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->positionate_right(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_position_bottom() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->positionate_bottom(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_position_vertical() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->center_vertical(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_position_horizontal() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->center_horizontal(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_align_top() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->fit_top(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_align_bottom() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->fit_bottom(clipping_->player()->info()->position());
    redraw_frame(true);

}

void CutterWindow::action_align_left() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->fit_left(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_align_right() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->fit_right(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_align_all() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->fit_all(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::action_norm_scale() {
    if (!is_paused(true)) {
        return;
    }
    clipping_->normalize_scale(clipping_->player()->info()->position());
    redraw_frame(true);
}

void CutterWindow::double_click(void *component) {
    if (component == viewer_) {
        action_properties();
    }
}

void CutterWindow::update_clipping_list() {
    in_key_list_ = true;
    char temp_buffer[100] = "";
    int selection = key_list_->value();
    uint32_t size = key_list_->size();

    key_list_->clear();
    uint32_t ref_frame = -1;
    clipping_->ref().get_reference_frame(&ref_frame);

    for (auto k : clipping_->keys()) {
        snprintf(temp_buffer, sizeof(temp_buffer),
            "%07u s:%4.2f r:%4.2lf (%u, %u) %s", k.frame, k.scale, k.angle(), k.px, k.py, ref_frame == k.frame ? " <r" : "");
        key_list_->add(temp_buffer);
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

    if (window->clipping_) {
        window->clipping_->player()->seek_frame(window->seek_bar_->value());
        window->update_buffers(true);
    }
}

void CutterWindow::redraw_frame(bool update_key_list) {
    if (update_key_list) {
        update_clipping_list();
        viewer_->update_preview(clipping_.get());
    }

    clipping_editor_->redraw();
    viewer_->redraw();
    update_seek_bar();
}

void CutterWindow::update_seek_bar() {
    if (!clipping_) {
        return;
    }

    in_seek_bar_callback_ = true;
    if (seek_bar_->maximum() != clipping_->player()->info()->count()) {
        seek_bar_->maximum(clipping_->player()->info()->count());
    }
    if (seek_bar_->value() !=  clipping_->player()->info()->position()) {
        seek_bar_->value(clipping_->player()->info()->position());
    }
    char temp[55] = "";
    snprintf(temp, sizeof(temp), "%u", clipping_->player()->info()->position());
    frame_input_->copy_label(temp);
    snprintf(temp, sizeof(temp), "%u", clipping_->player()->info()->count());
    frame_counter_->copy_label(temp);

    seconds_to_str(temp, sizeof(temp), clipping_->player()->info()->time());
    frame_time_->copy_label(temp);
    seconds_to_str(temp, sizeof(temp), clipping_->player()->info()->duration());
    video_duration_->copy_label(temp);

    in_seek_bar_callback_ = false;
}

void CutterWindow::key_list_auto_selection() {
    in_key_list_ = true;
    selected_clip_ = clipping_->find_index(clipping_->player()->info()->position()) + 1;
    if (selected_clip_ > 0 && selected_clip_ <= static_cast<uint32_t>(key_list_->size())) {
        key_list_->value(selected_clip_);
    }
    in_key_list_ = false;
}

void CutterWindow::update_buffers(bool frame_changed) {
    if (clipping_->player()->info()->error()) {
        return;
    }

    if (frame_changed) {
        update_seek_bar();
        redraw_frame();
        key_list_auto_selection();
        clipping_editor_->update(clipping_.get());
        viewer_->update_preview(clipping_.get());
    } else {
        clipping_editor_->draw_operations();
        if (clipping_editor_->key_changed(true)) {
            update_clipping_list();
            viewer_->update_preview(clipping_.get());
        }
    }

    if (clipping_->w() < 15 || clipping_->h() < 15) {
        clipping_->wh(clipping_->player()->info()->w(), clipping_->player()->info()->h());
    }

    if (clipping_->keys().size() != static_cast<u_int32_t>(key_list_->size())) {
        update_clipping_list();
    }
}

void CutterWindow::poll_actions() {
    if (clipping_) {
        update_buffers(clipping_->player()->frame_changed(true));
        if (wink_comparison_ && clipping_editor_->compare_box() && !clipping_->player()->is_playing()) {
            ++wink_lap_;
            if (wink_lap_ > 11) {
                wink_lap_ = 0;
                clipping_editor_->wink_compare_box();
            }
        }
    }
}

void CutterWindow::display_speed() {
    if (clipping_) {
        char temp[50];
        snprintf(temp, sizeof(temp), "%0.2f", clipping_->player()->get_speed());
        btn_speed_->copy_label(temp);
    }
}

void CutterWindow::pause_resume() {
    if (visible() && clipping_) {
        if (clipping_->player()->is_playing()) {
            clipping_->player()->pause();
        } else {
            clipping_->player()->play();
        }
    }
}

void CutterWindow::action_speed() {
    clipping_->player()->change_speed(!Fl::event_shift());
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

bool CutterWindow::compare_enabled() {
    return clipping_editor_->compare_box();
}

void CutterWindow::cancel_operations() {
    if (visible()) {
        clipping_editor_->cancel_operations();
    }
}


}  // namespace vcutter

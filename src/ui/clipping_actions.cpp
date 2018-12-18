/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/common/utils.h"
#include "src/ui/dialogs/file_dialog.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_cutter/options_window.h"

#include "src/ui/clipping_actions.h"

namespace vcutter {

ClippingActions2::ClippingActions2(ClippingActions2Handler * handler) {
    has_key_copy_ = false;
    handler_ = handler;
    active_cb_ = [this] () -> bool {
        return active();
    };
    restore_session();
}

ClippingActions2::~ClippingActions2() {

}

ClippingActions2Handler *ClippingActions2::handler() {
    return handler_;
}

bool ClippingActions2::has_copy() {
    return has_key_copy_;
}

bool ClippingActions2::has_copy(bool show_require_paused_message) {
    if (has_copy()) {
        return check_player_paused(show_require_paused_message);
    }

    return false;
}


Clipping *ClippingActions2::clipping() {
    return clipping_.get();
}

Player *ClippingActions2::player() {
    if (clipping_) {
        return clipping_->player();
    }
    return NULL;
}

void ClippingActions2::close() {
    clipping_.reset();
    has_key_copy_ = false;
}

bool ClippingActions2::open(const std::string& path, bool path_is_video) {
    clipping_.reset(new ClippingSession("cwnd", path.c_str(), path_is_video));
    return handle_opened_clipping();
}

bool ClippingActions2::open(const char *path) {
    std::string file_path;
    if (path) {
        file_path = path;
    } else {
        FileDialog dialog(filedialog_project_or_video, true);
        file_path = dialog.choose_path();
    }
    if (file_path.empty()) {
        return false;
    }
    return open(file_path, is_video_path(file_path));
}

bool ClippingActions2::handle_opened_clipping() {
    if (!clipping_->good()) {
        show_error("Não foi possivel abrir o arquivo de video.");
        handler_->handle_clipping_opened(false);
        return false;
    }
    clipping_->player()->seek_frame(clipping_->first_frame());
    handler_->handle_clipping_opened(true);
    return true;
}

void ClippingActions2::restore_session() {
    std::shared_ptr<ClippingSession> restored(std::move(ClippingSession::restore_session("cwnd")));

    if (!restored) {
        return;
    }

    clipping_.swap(restored);

    handle_opened_clipping();
}

bool ClippingActions2::check_player_paused(bool show_message) {
    if (!clipping()) {
        return false;
    }

    if (player()->is_playing()) {
        if (show_message) {
            show_error("This action requires that you pause the video before");
        }
        return false;
    }

    return true;
}


bool ClippingActions2::save() {
    if (!active()) {
        return false;
    }

    if (clipping()->saved_path().empty()) {
        return save_as();
    }

    clipping()->save(clipping()->saved_path().c_str());
    return true;
}

bool ClippingActions2::save_as() {
    if (!active()) {
        return false;
    }

    FileDialog dialog(filedialog_project, false);

    auto path = dialog.choose_path();
    if (!path.empty()) {
        if (filepath_exists(path.c_str()) && !ask("The file already exists. Overwrite it ?")) {
            return false;
        }
        clipping()->save(path.c_str());
    }

    return true;
}

bool ClippingActions2::active() {
    return clipping() != NULL;
}

action_ptr ClippingActions2::action_position_top() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->positionate_top(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}


action_ptr ClippingActions2::action_position_left() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->positionate_left(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_position_right() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->positionate_right(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_position_bottom() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->positionate_bottom(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_position_vertical() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->center_vertical(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_position_horizontal() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->center_horizontal(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_align_top() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->fit_top(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_align_bottom() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->fit_bottom(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_align_left() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->fit_left(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_align_right() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->fit_right(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_align_all() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->fit_all(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_norm_scale() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            clipping()->normalize_scale(player()->info()->position());
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_clear_keys() {
    return action_ptr(new Action(
        [this] () {
            if (clipping()->keys().empty()) {
                show_error("There is no keys to clear");
                return;
            }

            if (!ask("Are you sure to delete all keys ?")) {
                return;
            }

            clipping()->remove_all();

            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}


action_ptr ClippingActions2::action_properties() {
    return action_ptr(new Action(
        [this] () {
            if (!active()) {
                return;
            }

            unsigned int w = clipping()->w();
            unsigned int h = clipping()->h();
            if (CutterOptionsWindow::edit_properties(player()->info()->w(), player()->info()->h(), &w, &h)) {
                clipping()->wh(w, h);
                handler_->handle_clipping_resized();
            }
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_pause_resume() {
    return action_ptr(new Action(
        [this] () {
            if (active()) {
                if (player()->is_playing()) {
                    player()->pause();
                } else {
                    player()->play();
                }
            }
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_clear_copy() {
    return action_ptr(new Action(
        [this] () {
            has_key_copy_ = false;
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_copy() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }

            key_copy_ = clipping()->at(player()->info()->position());
            has_key_copy_ = true;
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_paste() {
    return action_ptr(new Action(
        [this] () {
            if (!has_copy("It's necessary to pause the video before paste a mark.")) {
                return;
            }
            auto copy = key_copy_;
            copy.frame = player()->info()->position();
            clipping()->add(copy);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_paste_rotation() {
    return action_ptr(new Action(
        [this] () {
            if (!has_copy("It's necessary to pause the video before paste the mark's rotation.")) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.angle(key_copy_.angle());
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_paste_scale() {
    return action_ptr(new Action(
        [this] () {
            if (!has_copy("It's necessary to pause the video before paste the mark's scale.")) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.scale = key_copy_.scale;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_paste_position(bool x, bool y) {
    return action_ptr(new Action(
        [this, x, y] () {
            if (!has_copy("It's necessary to pause the video before paste the mark's position.")) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            if (x)
                key.px = key_copy_.px;
            if (y)
                key.py = key_copy_.py;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_clear_rotation() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.angle(0);
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_clear_scale() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.scale = 1;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_scale_half() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.scale = 0.5;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_scale_half_2() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.scale = 0.25;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_scale_2() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.scale = 2;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_scale_3() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.scale = 3;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_clear_position() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.px = player()->info()->w() / 2;
            key.py = player()->info()->h() / 2;
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_rotation_90() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.angle(90);
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_rotation_180() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.angle(180);
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_rotation_270() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }
            auto key = clipping()->at(player()->info()->position());
            key.angle(270);
            clipping()->add(key);
            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_swap_wh() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }

            uint32_t video_w = player()->info()->w();
            uint32_t video_h = player()->info()->h();

            if (!video_h || !video_w) {
                show_error("Unxpected error width or height invalid in this video");
                return;
            }

            if (!ask("Are you sure to swap width/height ?\nThis may resize the canvas.\nThis will rotate +90 degrees")) {
                return;
            }

            uint32_t width = clipping()->w();
            uint32_t height = clipping()->h();
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

            clipping()->wh(height, width);

            for (auto k : clipping()->keys()) {
                k.angle(k.angle() + 90);
                k.scale *= (1.0 / scaled);
                clipping()->add(k);
            }

            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_rotate_all_180() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }

            if (!ask("Are you sure to rotate all keys 180 degrees ?")) {
                return;
            }

            for (auto k : clipping()->keys()) {
                k.angle(k.angle() + 180);
                clipping()->add(k);
            }

            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_scale_all() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
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

            for (auto k : clipping()->keys()) {
                k.scale *= scale;
                clipping()->add(k);
            }

            handler_->handle_clipping_keys_changed();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_next() {
    return action_ptr(new Action(
        [this] () {
            if (!player()->is_playing() && Fl::event_shift() && player()->info()->position() + 33 < player()->info()->count()) {
                player()->seek_frame(player()->info()->position() + 33);
                handler_->handle_buffer_modified();
                return;
            }
            player()->next();
            handler_->handle_buffer_modified();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_prior() {
    return action_ptr(new Action(
        [this] () {
            if (!player()->is_playing() && Fl::event_shift() && player()->info()->position() - 33 > 0) {
                player()->seek_frame(player()->info()->position() - 33);
                handler_->handle_buffer_modified();
                return;
            }

            player()->prior();
            handler_->handle_buffer_modified();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_insert() {
    return action_ptr(new Action(
        [this] () {
            if (!check_player_paused(true)) {
                return;
            }

            auto key = clipping()->at(player()->info()->position());

            if (key.computed()) {
                clipping()->add(key);
                handler_->handle_clipping_keys_changed();
            } else {
                show_error("There already is a mark at this frame.");
            }
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_delete() {
    return action_ptr(new Action(
        [this] () {
            if (!clipping()) {
                return;
            }
            if (player()->is_playing()) {
                show_error("It's necessary to pause the video before delete a mark.");
                return;
            }
            if (clipping()->keys().size() < 1) {
                show_error("There is no item to remove.");
                return;
            }

            auto key = clipping()->at(player()->info()->position());

            if (key.computed()) {
                show_error("The current video's frame does not have a mark to remove.");
                return;
            }

            if (ask("Are you sure to delete current")) {
                clipping()->remove(key.frame);
                handler_->handle_clipping_keys_changed();
            }
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_cutoff1() {
    return action_ptr(new Action(
        [this] () {
            if (!clipping()) {
                return;
            }
            if (player()->is_playing()) {
                show_error("It's necessary to pause the video before cut.");
                return;
            }
            if (ask("This action will remove several marks from begin. Are you sure ?")) {
                clipping()->define_start(player()->info()->position());
                handler_->handle_clipping_keys_changed();
            }
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_cutoff12() {
    return action_ptr(new Action(
        [this] () {
            if (!clipping()) {
                return;
            }
            if (player()->is_playing()) {
                show_error("It's necessary to pause the video before cut.");
                return;
            }
            if (ask("This action will remove several marks. Are you sure ?")) {
                clipping()->remove_all(player()->info()->position());
                handler_->handle_clipping_keys_changed();
            }
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_cutoff2() {
    return action_ptr(new Action(
        [this] () {
            if (!clipping()) {
                return;
            }

            if (player()->is_playing()) {
                show_error("It's necessary to pause the video before cut.");
                return;
            }

            if (ask("This action will remove several marks to the end. Are you sure ?")) {
                clipping()->define_end(player()->info()->position());
                handler_->handle_clipping_keys_changed();
            }
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_play() {
    return action_ptr(new Action(
        [this] () {
            if (!active()) {
                return;
            }

            player()->play();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_stop() {
    return action_ptr(new Action(
        [this] () {
            if (!active()) {
                return;
            }

            player()->stop();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_pause() {
    return action_ptr(new Action(
        [this] () {
            if (!active()) {
                return;
            }

            player()->pause();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_search() {
    return action_ptr(new Action(
        [this] () {
            if (!active()) {
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
                handler_->handle_buffer_modified();
                return;
            } else if (sscanf(timestr, "%d", &seconds) != 1) {
                return;
            }

            player()->seek_time((seconds + 1) * 1000);
            handler_->handle_buffer_modified();
        },
        active_cb_
    ));
}

action_ptr ClippingActions2::action_play_interval() {
    return action_ptr(new Action(
        [this] () {
            if (!active()) {
                return;
            }

            if (clipping()->keys().empty()) {
                return;
            }

            player()->play(clipping()->first_frame(), clipping()->last_frame());
        },
        active_cb_
    ));
}


}  // namespace vcutter

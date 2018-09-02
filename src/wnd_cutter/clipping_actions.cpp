/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/common/utils.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_cutter/options_window.h"

#include "src/wnd_cutter/clipping_actions.h"

namespace vcutter {

ClippingActions::ClippingActions(ClippingActionsHandler * handler) {
    has_key_copy_ = false;
    handler_ = handler;
}

ClippingActions::~ClippingActions() {

}

bool ClippingActions::has_copy() {
    return has_key_copy_;
}

bool ClippingActions::has_copy(bool show_require_paused_message) {
    if (has_copy()) {
        return check_player_paused(show_require_paused_message);
    }

    return false;
}


Clipping *ClippingActions::clipping() {
    return clipping_.get();
}

PlayerWrapper *ClippingActions::player() {
    if (clipping_) {
        return clipping_->player();
    }
    return NULL;
}

void ClippingActions::close() {
    clipping_.reset();
    has_key_copy_ = false;
}

bool ClippingActions::open(const std::string& path, bool path_is_video) {
    clipping_.reset(new ClippingSession("cwnd", path.c_str(), path_is_video));
    return handle_opened_clipping();
}

bool ClippingActions::handle_opened_clipping() {
    if (!clipping_->good()) {
        show_error("Não foi possivel abrir o arquivo de video.");
        handler_->handle_clipping_opened(false);
        return false;
    }
    clipping_->player()->seek_frame(clipping_->first_frame());
    handler_->handle_clipping_opened(true);
    return true;
}

bool ClippingActions::restore_session() {
    std::shared_ptr<ClippingSession> restored(std::move(ClippingSession::restore_session("cwnd")));

    if (!restored) {
        return false;
    }

    clipping_.swap(restored);

    return handle_opened_clipping();
}

bool ClippingActions::check_player_paused(bool show_message) {
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


bool ClippingActions::save(History *history) {
    if (!active()) {
        return true;
    }

    if (clipping()->saved_path().empty()) {
        return save_as(history);
    }

    clipping()->save(clipping()->saved_path().c_str());
    return true;
}

bool ClippingActions::save_as(History *history) {
    if (!active()) {
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

    clipping()->save(path.c_str());
    return true;
}

bool ClippingActions::active() {
    return handler_->player_bar_active() && clipping_;
}

menu_callback_t ClippingActions::action_position_top() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->positionate_top(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}


menu_callback_t ClippingActions::action_position_left() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->positionate_left(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_position_right() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->positionate_right(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_position_bottom() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->positionate_bottom(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_position_vertical() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->center_vertical(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_position_horizontal() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->center_horizontal(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_align_top() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->fit_top(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_align_bottom() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->fit_bottom(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };

}

menu_callback_t ClippingActions::action_align_left() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->fit_left(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_align_right() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->fit_right(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_align_all() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->fit_all(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_norm_scale() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        clipping()->normalize_scale(player()->info()->position());
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_clear_keys() {
    return [this] () {
        if (clipping()->keys().empty()) {
            show_error("There is no keys to clear");
            return;
        }

        if (!ask("Are you sure to delete all keys ?")) {
            return;
        }

        clipping()->remove_all();

        handler_->handle_clipping_keys_changed();
    };
}


menu_callback_t ClippingActions::action_properties() {
    return [this] () {
        if (!active()) {
            return;
        }

        unsigned int w = clipping()->w();
        unsigned int h = clipping()->h();
        if (CutterOptionsWindow::edit_properties(player()->info()->w(), player()->info()->h(), &w, &h)) {
            clipping()->wh(w, h);
            handler_->handle_clipping_resized();
        }
    };
}

menu_callback_t ClippingActions::action_pause_resume() {
    return [this] () {
        if (active()) {
            if (player()->is_playing()) {
                player()->pause();
            } else {
                player()->play();
            }
        }
    };
}

menu_callback_t ClippingActions::action_clear_copy() {
    return [this] () {
        has_key_copy_ = false;
    };
}

menu_callback_t ClippingActions::action_copy() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }

        key_copy_ = clipping()->at(player()->info()->position());
        has_key_copy_ = true;
    };
}

menu_callback_t ClippingActions::action_paste() {
    return [this] () {
        if (!has_copy("It's necessary to pause the video before paste a mark.")) {
            return;
        }
        auto copy = key_copy_;
        copy.frame = player()->info()->position();
        clipping()->add(copy);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_paste_rotation() {
    return [this] () {
        if (!has_copy("It's necessary to pause the video before paste the mark's rotation.")) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.angle(key_copy_.angle());
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_paste_scale() {
    return [this] () {
        if (!has_copy("It's necessary to pause the video before paste the mark's scale.")) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.scale = key_copy_.scale;
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_paste_position(bool x, bool y) {
    return [this, x, y] () {
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
    };
}

menu_callback_t ClippingActions::action_clear_rotation() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.angle(0);
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_clear_scale() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.scale = 1;
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_scale_half() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.scale = 0.5;
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_scale_half_2() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.scale = 0.25;
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_scale_2() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.scale = 2;
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_scale_3() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.scale = 3;
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_clear_position() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.px = player()->info()->w() / 2;
        key.py = player()->info()->h() / 2;
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_rotation_90() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.angle(90);
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_rotation_180() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.angle(180);
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_rotation_270() {
    return [this] () {
        if (!check_player_paused(true)) {
            return;
        }
        auto key = clipping()->at(player()->info()->position());
        key.angle(270);
        clipping()->add(key);
        handler_->handle_clipping_keys_changed();
    };
}

menu_callback_t ClippingActions::action_swap_wh() {
    return [this] () {
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
    };
}

menu_callback_t ClippingActions::action_rotate_all_180() {
    return [this] () {
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
    };
}

menu_callback_t ClippingActions::action_scale_all() {
    return [this] () {
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
    };
}

menu_callback_t ClippingActions::action_prior() {
    return [this] () {
        if (!player()->is_playing() && Fl::event_shift() && player()->info()->position() + 33 < player()->info()->count()) {
            player()->seek_frame(player()->info()->position() + 33);
            handler_->handle_buffer_modified();
            return;
        }
        player()->next();
        handler_->handle_buffer_modified();
    };
}

menu_callback_t ClippingActions::action_next() {
    return [this] () {
        if (!player()->is_playing() && Fl::event_shift() && player()->info()->position() - 33 > 0) {
            player()->seek_frame(player()->info()->position() - 33);
            handler_->handle_buffer_modified();
            return;
        }

        player()->prior();
        handler_->handle_buffer_modified();

    };
}

menu_callback_t ClippingActions::action_insert() {
    return [this] () {
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
    };
}

menu_callback_t ClippingActions::action_delete() {
    return [this] () {
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
    };
}

menu_callback_t ClippingActions::action_cutoff1() {
    return [this] () {
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
    };
}

menu_callback_t ClippingActions::action_cutoff12() {
    return [this] () {
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
    };
}

menu_callback_t ClippingActions::action_cutoff2() {
    return [this] () {
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
    };
}


}  // namespace vcutter

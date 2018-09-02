/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/common/utils.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_cutter/options_window.h"

#include "src/wnd_cutter/clipping_actions.h"

namespace vcutter {

ClippingActions::ClippingActions(ClippingActionsHandler * handler) {
    handler_ = handler;
}

ClippingActions::~ClippingActions() {

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

}  // namespace vcutter

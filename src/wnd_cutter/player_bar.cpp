/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/common/utils.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_cutter/player_bar.h"

namespace vcutter {

PlayerBar::PlayerBar(PlayerBarHandler * handler) {
    handler_ = handler;
}

PlayerBar::~PlayerBar() {

}

Clipping *PlayerBar::clipping() {
    return clipping_.get();
}

PlayerWrapper *PlayerBar::player() {
    if (clipping_) {
        return clipping_->player();
    }
    return NULL;
}

void PlayerBar::close() {
    clipping_.reset();
}

bool PlayerBar::open(const std::string& path, bool path_is_video) {
    clipping_.reset(new ClippingSession("cwnd", path.c_str(), path_is_video));
    return handle_opened_clipping();
}

bool PlayerBar::handle_opened_clipping() {
    if (!clipping_->good()) {
        show_error("Não foi possivel abrir o arquivo de video.");
        handler_->handle_clipping_opened(false);
        return false;
    }
    clipping_->player()->seek_frame(clipping_->first_frame());
    handler_->handle_clipping_opened(true);
    return true;
}

bool PlayerBar::restore_session() {
    std::shared_ptr<ClippingSession> restored(std::move(ClippingSession::restore_session("cwnd")));

    if (!restored) {
        return false;
    }

    clipping_.swap(restored);

    return handle_opened_clipping();
}

bool PlayerBar::check_player_paused(bool show_message) {
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


bool PlayerBar::save(History *history) {
    if (!clipping()) {
        return true;
    }

    if (clipping()->saved_path().empty()) {
        return save_as(history);
    }

    clipping()->save(clipping()->saved_path().c_str());
    return true;
}

bool PlayerBar::save_as(History *history) {
    if (!clipping()) {
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


}  // namespace vcutter

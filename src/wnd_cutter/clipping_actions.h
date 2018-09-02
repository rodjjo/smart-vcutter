/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_CUTTER_CLIPPING_ACTIONS_H_
#define SRC_WND_CUTTER_CLIPPING_ACTIONS_H_

#include <memory>
#include "src/data/history.h"
#include "src/clippings/clipping_session.h"
#include "src/wnd_main/menu_bar.h"

namespace vcutter {

class ClippingActionsHandler {
  public:
    virtual ~ClippingActionsHandler() {}
    virtual bool player_bar_active() = 0;
    virtual void handle_clipping_opened(bool opened) = 0;
    virtual void handle_clipping_resized() = 0;
    virtual void handle_clipping_keys_changed() = 0;
};

class ClippingActions {
 public:
    ClippingActions(ClippingActionsHandler * handler);
    virtual ~ClippingActions();
    PlayerWrapper *player();
    void close();
    Clipping *clipping();
    bool open(const std::string& path, bool path_is_video);
    bool restore_session();
    bool handle_opened_clipping();
    bool check_player_paused(bool show_message);
    bool save(History * history);
    bool save_as(History * history);
    menu_callback_t action_position_top();
    menu_callback_t action_position_left();
    menu_callback_t action_position_right();
    menu_callback_t action_position_bottom();
    menu_callback_t action_position_vertical();
    menu_callback_t action_position_horizontal();
    menu_callback_t action_align_top();
    menu_callback_t action_align_bottom();
    menu_callback_t action_align_left();
    menu_callback_t action_align_right();
    menu_callback_t action_align_all();
    menu_callback_t action_norm_scale();
    menu_callback_t action_clear_keys();
    menu_callback_t action_properties();
 private:
    bool active();
 private:
    ClippingActionsHandler * handler_;
    std::shared_ptr<ClippingSession> clipping_;
};

}  // namespace vcutter

#endif  // SRC_WND_CUTTER_CLIPPING_ACTIONS_H_

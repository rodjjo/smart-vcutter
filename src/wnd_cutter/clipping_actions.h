/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_CUTTER_CLIPPING_ACTIONS_H_
#define SRC_WND_CUTTER_CLIPPING_ACTIONS_H_

#include <memory>
#include "src/data/history.h"
#include "src/clippings/clipping_session.h"
#include "src/wnd_main/callbacks.h"

namespace vcutter {

class ClippingActionsHandler {
  public:
    virtual ~ClippingActionsHandler() {}
    virtual bool clipping_actions_active() = 0;
    virtual void handle_clipping_opened(bool opened) = 0;
    virtual void handle_clipping_resized() = 0;
    virtual void handle_clipping_keys_changed() = 0;
    virtual void handle_buffer_modified() = 0;
    virtual void handle_frame_changed(Player *player) = 0;
};

class ClippingActions {
 public:
    ClippingActions(ClippingActionsHandler * handler);
    virtual ~ClippingActions();
    Player *player();
    ClippingActionsHandler *handler();
    void close();
    Clipping *clipping();
    bool open(const std::string& path, bool path_is_video);
    bool restore_session();
    bool handle_opened_clipping();
    bool check_player_paused(bool show_message);
    bool save(History * history);
    bool save_as(History * history);

    bool has_copy();
    bool has_copy(bool show_require_paused_message);

    callback_t action_play();
    callback_t action_stop();
    callback_t action_pause();
    callback_t action_search();
    callback_t action_position_top();
    callback_t action_position_left();
    callback_t action_position_right();
    callback_t action_position_bottom();
    callback_t action_position_vertical();
    callback_t action_position_horizontal();
    callback_t action_align_top();
    callback_t action_align_bottom();
    callback_t action_align_left();
    callback_t action_align_right();
    callback_t action_align_all();
    callback_t action_norm_scale();
    callback_t action_clear_keys();
    callback_t action_properties();
    callback_t action_pause_resume();
    callback_t action_clear_copy();
    callback_t action_copy();
    callback_t action_paste();
    callback_t action_paste_rotation();
    callback_t action_paste_scale();
    callback_t action_paste_position(bool x, bool y);
    callback_t action_clear_rotation();
    callback_t action_clear_scale();
    callback_t action_scale_half();
    callback_t action_scale_half_2();
    callback_t action_scale_2();
    callback_t action_scale_3();
    callback_t action_clear_position();
    callback_t action_rotation_90();
    callback_t action_rotation_180();
    callback_t action_rotation_270();
    callback_t action_swap_wh();
    callback_t action_rotate_all_180();
    callback_t action_scale_all();
    callback_t action_prior();
    callback_t action_next();
    callback_t action_insert();
    callback_t action_delete();
    callback_t action_cutoff1();
    callback_t action_cutoff12();
    callback_t action_cutoff2();
    callback_t action_play_interval();

 private:
    bool active();
 private:
    bool has_key_copy_;
    ClippingKey key_copy_;
    ClippingActionsHandler * handler_;
    std::shared_ptr<ClippingSession> clipping_;
};

}  // namespace vcutter

#endif  // SRC_WND_CUTTER_CLIPPING_ACTIONS_H_

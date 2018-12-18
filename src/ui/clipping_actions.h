/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_UI_CLIPPING_ACTIONS_H_
#define SRC_UI_CLIPPING_ACTIONS_H_

#include <memory>
#include <inttypes.h>
#include "src/data/history.h"
#include "src/clippings/clipping_session.h"
#include "src/ui/action.h"

namespace vcutter {

class ClippingActions2Handler {
  public:
    virtual ~ClippingActions2Handler() {}
    virtual void handle_clipping_opened(bool opened) = 0;
    virtual void handle_clipping_resized() = 0;
    virtual void handle_clipping_keys_changed() = 0;
    virtual void handle_buffer_modified() = 0;
};

class ClippingActions2 {
 public:
    ClippingActions2(ClippingActions2Handler * handler);
    virtual ~ClippingActions2();
    Player *player();
    ClippingActions2Handler *handler();
    void close();
    Clipping *clipping();
    bool open(const char *path = NULL);
    bool open(const std::string& path, bool path_is_video);
    bool handle_opened_clipping();
    bool check_player_paused(bool show_message);
    bool save();
    bool save_as();

    bool has_copy();
    bool has_copy(bool show_require_paused_message);

    action_ptr action_play();
    action_ptr action_stop();
    action_ptr action_pause();
    action_ptr action_search();
    action_ptr action_position_top();
    action_ptr action_position_left();
    action_ptr action_position_right();
    action_ptr action_position_bottom();
    action_ptr action_position_vertical();
    action_ptr action_position_horizontal();
    action_ptr action_align_top();
    action_ptr action_align_bottom();
    action_ptr action_align_left();
    action_ptr action_align_right();
    action_ptr action_align_all();
    action_ptr action_norm_scale();
    action_ptr action_clear_keys();
    action_ptr action_properties();
    action_ptr action_pause_resume();
    action_ptr action_clear_copy();
    action_ptr action_copy();
    action_ptr action_paste();
    action_ptr action_paste_rotation();
    action_ptr action_paste_scale();
    action_ptr action_paste_position(bool x, bool y);
    action_ptr action_clear_rotation();
    action_ptr action_clear_scale();
    action_ptr action_scale_half();
    action_ptr action_scale_half_2();
    action_ptr action_scale_2();
    action_ptr action_scale_3();
    action_ptr action_clear_position();
    action_ptr action_rotation_90();
    action_ptr action_rotation_180();
    action_ptr action_rotation_270();
    action_ptr action_swap_wh();
    action_ptr action_rotate_all_180();
    action_ptr action_scale_all();
    action_ptr action_prior();
    action_ptr action_next();
    action_ptr action_insert();
    action_ptr action_delete();
    action_ptr action_cutoff1();
    action_ptr action_cutoff12();
    action_ptr action_cutoff2();
    action_ptr action_play_interval();

 private:
    bool active();
    void restore_session();

 private:
    bool_callback_t active_cb_;
    bool has_key_copy_;
    ClippingKey key_copy_;
    ClippingActions2Handler * handler_;
    std::shared_ptr<ClippingSession> clipping_;
};

}  // namespace vcutter

#endif  // SRC_UI_CLIPPING_ACTIONS_H_

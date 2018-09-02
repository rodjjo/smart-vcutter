/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_CUTTER_PLAYER_BAR_H_
#define SRC_WND_CUTTER_PLAYER_BAR_H_

#include <memory>
#include "src/data/history.h"
#include "src/clippings/clipping_session.h"


namespace vcutter {

class PlayerBarHandler {
  public:
    virtual ~PlayerBarHandler() {}
    virtual void handle_clipping_opened(bool opened) = 0;
    virtual void handle_redraw_needed() = 0;
};

class PlayerBar {
 public:
    PlayerBar(PlayerBarHandler * handler);
    virtual ~PlayerBar();
    PlayerWrapper *player();
    void close();
    Clipping *clipping();
    bool open(const std::string& path, bool path_is_video);
    bool restore_session();
    bool handle_opened_clipping();
    bool check_player_paused(bool show_message);
    bool save(History * history);
    bool save_as(History * history);

 private:
    PlayerBarHandler * handler_;
    std::shared_ptr<ClippingSession> clipping_;
};

}  // namespace vcutter

#endif  // SRC_WND_CUTTER_PLAYER_BAR_H_

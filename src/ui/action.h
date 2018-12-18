/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_UI_ACTION_H_
#define SRC_UI_ACTION_H_

#include <memory>
#include "src/common/callbacks.h"

namespace vcutter {

class Action {
 public:
    Action(callback_t handler);
    Action(callback_t handler, bool_callback_t check_active);
    virtual ~Action(){}
    bool enabled();
    void execute();
    static std::shared_ptr<Action> null_action();
private:
    callback_t handler_;
    bool_callback_t check_active_;
};


typedef std::shared_ptr<Action> action_ptr;

}  // namespace vcutter

#endif // SRC_UI_ACTION_H_

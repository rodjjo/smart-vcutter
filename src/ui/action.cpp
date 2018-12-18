/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/ui/action.h"

namespace vcutter {


Action::Action(callback_t handler) {
    handler_ = handler;
}

Action::Action(callback_t handler, bool_callback_t check_active) {
    handler_ = handler;
    check_active_ = check_active;
}

bool Action::enabled() {
    if (check_active_) {
        return check_active_();
    }
    return true;
}

void Action::execute() {
    if (enabled()) {
        handler_();
    }
}

std::shared_ptr<Action> Action::null_action() {
    return std::shared_ptr<Action>(new Action([](){}));
}


}  // namespace vcutter

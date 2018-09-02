/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <FL/Fl.H>
#include "src/wnd_main/menu_bar.h"

namespace vcutter {

MenuBar::MenuBar(int w, int h, callback_t callback) : Fl_Menu_Bar(0, 0, w, h) {
    callback_ = callback;
}

MenuBar::~MenuBar() {
}

int MenuBar::handle(int value) {
    if (value == FL_PUSH && callback_ && Fl::event_button() == FL_LEFT_MOUSE) {
        callback_();
    }

    return Fl_Menu_Bar::handle(value);
}

}  // namespace vcutter

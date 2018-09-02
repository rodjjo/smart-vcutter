/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_MAIN_MENU_BAR_H_
#define SRC_WND_MAIN_MENU_BAR_H_


#include <FL/Fl_Menu_Bar.H>

#include "src/wnd_main/callbacks.h"

namespace vcutter {

class MenuBar: public Fl_Menu_Bar {
 public:
    MenuBar(int w, int h, callback_t callback);
    virtual ~MenuBar();
    int handle(int value) override;
 private:
    callback_t callback_;
};

}  // namespace vcutter

#endif  // SRC_WND_MAIN_MENU_BAR_H_

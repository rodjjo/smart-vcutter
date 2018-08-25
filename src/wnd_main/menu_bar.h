/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_MAIN_MENU_BAR_H_
#define SRC_WND_MAIN_MENU_BAR_H_

#include <functional>
#include <FL/Fl_Menu_Bar.H>

namespace vcutter {

typedef std::function<void()> menu_callback_t;

class MenuBar: public Fl_Menu_Bar {
 public:
    MenuBar(int w, int h, menu_callback_t callback);
    virtual ~MenuBar();
    int handle(int value) override;
 private:
    menu_callback_t callback_;
};

}  // namespace vcutter

#endif  // SRC_WND_MAIN_MENU_BAR_H_

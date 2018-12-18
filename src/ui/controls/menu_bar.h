/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_UI_CONTROLS_MENU_BAR_H_
#define SRC_UI_CONTROLS_MENU_BAR_H_

#include <memory>
#include <list>
#include <FL/Fl_Menu_Bar.H>

#include "src/data/xpm.h"
#include "src/ui/action.h"



namespace vcutter {


class MenuItem;

class MenuBar2 : public Fl_Menu_Bar {
 public:
    MenuBar2(int w, int h);
    virtual ~MenuBar2();
    void invalidate();
    void add(const char *label, std::shared_ptr<Action> action, xpm::xpm_t icon=xpm::no_image);
    int handle(int value) override;
 private:
    std::list<std::shared_ptr<MenuItem> > items_;
};


}  // namespace vcutter

#endif  // SRC_UI_CONTROLS_MENU_BAR_H_

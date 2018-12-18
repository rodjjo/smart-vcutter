/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/ui/controls/menu_item.h"
#include "src/ui/controls/menu_bar.h"


namespace vcutter {

MenuBar2::MenuBar2(int w, int h) : Fl_Menu_Bar(0, 0, w, h) {
}

MenuBar2::~MenuBar2() {

}

int MenuBar2::handle(int value) {
    invalidate();
    return Fl_Menu_Bar::handle(value);
}

void MenuBar2::invalidate() {
    for (auto item : items_) {
        item->invalidate();
    }
}

void MenuBar2::add(const char *label, std::shared_ptr<Action> action, xpm::xpm_t icon) {
    items_.push_back(std::shared_ptr<MenuItem>(
        new MenuItem(this, label, action, icon)
    ));
}



}  // namespace vcutter

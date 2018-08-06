/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include "smart-vcutter/wnd_main/menu.h"

namespace vcutter {

Menu::Menu(Fl_Menu_Bar *menu_bar, const char *label) {
    path_ = label;
    group_ = 0;
    menu_bar_ = menu_bar;
}

Menu::Menu(int group, menu_callback_t callback) {
    group_ = group;
    callback_ = callback;
    index_ = 0;
    menu_bar_ = NULL;
}

Menu::~Menu() {
}

Menu *Menu::add(const char *label, const char *shortcut, menu_callback_t callback, int flags, int group, xpm::xpm_t icon) {
    std::shared_ptr<Menu> menu_item(new Menu(group, callback));
    menu_item->path_ = path_ + "/" + label;
    menu_item->menu_bar_ = menu_bar_;
    menu_bar_->add(menu_item->path_.c_str(), shortcut, Menu::menu_action, menu_item.get(), flags);
    if (icon != xpm::no_image) {
        auto item = const_cast<Fl_Menu_Item *>(menu_bar_->find_item((menu_item->path_.c_str())));
        menu_item->img_ = xpm::image(icon);
        menu_item->label_text_ = std::string(" ") + item->text;
        menu_item->multi_label_.typea = _FL_IMAGE_LABEL;
        menu_item->multi_label_.labela = reinterpret_cast<char *>(menu_item->img_.get());
        menu_item->multi_label_.typeb = FL_NORMAL_LABEL;
        menu_item->multi_label_.labelb = menu_item->label_text_.c_str();
        item->image(menu_item->img_.get());
        menu_item->multi_label_.label(item);
        char buffer[1025] = "";
        if (menu_bar_->item_pathname(buffer, sizeof(buffer) - 1, item) == 0) {
            menu_item->path_ = buffer;
        }
    }
    items_.push_back(menu_item);
    return menu_item.get();
}

void Menu::enable(bool enabled, int group) {
    for (auto m : items_) {
        if (m->group_ == group || (m->group_ & group) != 0) {
            auto instance = const_cast<Fl_Menu_Item *>(menu_bar_->find_item((m->path_.c_str())));
            if (instance) {
                if (enabled) {
                    instance->activate();
                } else {
                    instance->deactivate();
                }
            }
        }
        m->enable(enabled, group);
    }
}

void Menu::menu_action(Fl_Widget *widget, void *user_data) {
    static_cast<Menu*>(user_data)->callback_(static_cast<Menu*>(user_data));
}

}  // namespace vcutter

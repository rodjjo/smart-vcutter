/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <string.h>
#include "src/wnd_main/menu.h"

namespace vcutter {

Menu::Menu(Fl_Menu_Bar *menu_bar, const char *label) {
    init(menu_bar, label);
}

Menu::Menu(Fl_Menu_Bar *menu_bar, const char *label, menu_callback_t callback) {
    init(menu_bar, label);
    callback_ = callback;
}

Menu::Menu(Fl_Menu_Bar *menu_bar, const char *path, int group, menu_callback_t callback) {
    group_ = group;
    path_ = path;
    callback_ = callback;
    menu_bar_ = menu_bar;
}

void Menu::init(Fl_Menu_Bar *menu_bar, const char *label) {
    path_ = label;
    group_ = 0;
    menu_bar_ = menu_bar;
}

Menu::~Menu() {
}

Menu *Menu::add(const char *label, const char *shortcut, menu_callback_t callback, int flags, int group, xpm::xpm_t icon) {
    std::shared_ptr<Menu> menu_item(new Menu(menu_bar_, (path_ + "/" + label).c_str(), group, callback));

    menu_bar_->add(menu_item->path_.c_str(), shortcut, Menu::menu_action, menu_item.get(), flags);

    menu_item->define_image(icon);

    define_root_callback();

    items_.push_back(menu_item);

    return menu_item.get();
}

void Menu::define_root_callback() {
    if (path_.find('/') != std::string::npos || !callback_) {
        return;
    }
    const Fl_Menu_Item *root_item = menu_bar_->find_item(path_.c_str());
    if (!root_item || root_item->callback()) {
        return;
    }
    const_cast<Fl_Menu_Item *>(root_item)->callback(&Menu::menu_action, this);
}

void Menu::define_image(xpm::xpm_t icon) {
    if (icon == xpm::no_image) {
        return;
    }

    auto item = const_cast<Fl_Menu_Item *>(menu_bar_->find_item((path_.c_str())));

    img_ = xpm::image(icon);
    label_text_ = std::string(" ") + item->text;
    multi_label_.typea = _FL_IMAGE_LABEL;
    multi_label_.labela = reinterpret_cast<char *>(img_.get());
    multi_label_.typeb = FL_NORMAL_LABEL;
    multi_label_.labelb = label_text_.c_str();
    item->image(img_.get());
    multi_label_.label(item);

    update_path(item);
}

void Menu::update_path(Fl_Menu_Item *item) {
    char buffer[1025] = "";
    if (menu_bar_->item_pathname(buffer, sizeof(buffer) - 1, item) == 0) {
        path_ = buffer;
    }
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
    static_cast<Menu*>(user_data)->callback_();
}

}  // namespace vcutter

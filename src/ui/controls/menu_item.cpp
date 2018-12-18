/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <regex> // NOLINT
#include "src/ui/controls/menu_bar.h"
#include "src/ui/controls/menu_item.h"


namespace vcutter {

std::pair<std::string, std::string> label_shortcut(const std::string& text) {
    std::regex short_cut_regex("^(.*)\\[([^\\]]+)\\](_$|$)", std::regex_constants::icase);
    std::smatch capture;

    if (std::regex_search(text, capture, short_cut_regex)) {
        return std::make_pair(capture[1].str() + capture[3].str(), capture[2].str());
    }

    return std::make_pair(text, std::string());
}

std::pair<std::string, int> label_flag(const std::string& text) {
    if (*text.rbegin() == '_') {
        return std::pair<std::string, int>(text.substr(0, text.size() - 1), FL_MENU_DIVIDER);
    }
    return std::make_pair(text, 0);
}

MenuItem::MenuItem(
    MenuBar2 *bar,
    const char *label,
    std::shared_ptr<Action> action,
    xpm::xpm_t icon
) {
    bar_ = bar;
    action_ = action;

    auto label_texts = label_shortcut(label);
    auto label_data = label_flag(label_texts.first);

    label_ = label_data.first;
    shortcut_ = label_texts.second;

    if (icon != xpm::no_image) {
        img_ = xpm::image(icon);
    }
    static_cast<Fl_Menu_Bar *>(bar_)->add(
        label_.c_str(),
        shortcut_.c_str(),
        MenuItem::callback, this,
        label_data.second);

    path_ = label_;

    if (img_) {
        auto item = const_cast<Fl_Menu_Item *>(bar_->find_item((label_.c_str())));
        multi_label_.typea = _FL_IMAGE_LABEL;
        multi_label_.labela = reinterpret_cast<char *>(img_.get());
        multi_label_.typeb = FL_NORMAL_LABEL;
        multi_label_.labelb = label_.c_str();
        item->image(img_.get());
        multi_label_.label(item);

        char buffer[1025] = "";
        if (bar_->item_pathname(buffer, sizeof(buffer) - 1, item) == 0) {
            path_ = buffer;
        }
    }
}


MenuItem::~MenuItem() {
}

void MenuItem::callback(Fl_Widget *widget, void *user_data) {
    auto item = static_cast<MenuItem *>(user_data);
    if (item->action_) {
        item->action_->execute();
    }
}

void MenuItem::invalidate() {
    if (action_) {
        auto item = const_cast<Fl_Menu_Item *>(bar_->find_item((path_.c_str())));
        if (action_->enabled()) {
            item->activate();
        } else {
            item->deactivate();
        }
    }
}


}  // namespace vcutter

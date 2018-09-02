/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_MAIN_MENU_H_
#define SRC_WND_MAIN_MENU_H_

#include <functional>
#include <memory>
#include <list>
#include <string>

#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Multi_Label.H>

#include "src/wnd_main/menu_bar.h"
#include "src/data/xpm.h"


namespace vcutter {

class Menu {
 public:
    Menu(MenuBar *menu_bar, const char *label, callback_t callback);
    Menu(MenuBar *menu_bar, const char *label);
    virtual ~Menu();
    Menu *add(const char *label, const char *shortcut, callback_t callback, int flags=0, int group=0, xpm::xpm_t icon=xpm::no_image);
    void enable(bool enabled, int group = 0);
    void check(bool checked);
 private:
    Fl_Menu_Item *menu_item();
    Menu(MenuBar *menu_bar, const char *path, int group, callback_t callback);
    static void menu_action(Fl_Widget *widget, void *user_data);
    void init(MenuBar *menu_bar, const char *label);
    void define_image(xpm::xpm_t icon);
    void define_root_callback();
    void update_path(Fl_Menu_Item *item);

 private:
    std::string path_;
    MenuBar *menu_bar_;
    int group_;
    callback_t callback_;
    std::list<std::shared_ptr<Menu> > items_;
    std::shared_ptr<Fl_RGB_Image> img_;
    std::string label_text_;
    Fl_Multi_Label multi_label_;
};


}  // namespace vcutter

#endif  // SRC_WND_MAIN_MENU_H_

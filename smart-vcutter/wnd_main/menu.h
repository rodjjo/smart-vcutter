/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_MENU_H
#define SMART_VCUTTER_MENU_H

#include <functional>
#include <memory>
#include <list>
#include <string>

#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Multi_Label.H>
#include "smart-vcutter/data/xpm.h"


namespace vcutter {

class Menu;

typedef std::function<void(Menu *menu_item)> menu_callback_t;

class Menu {
 public:
   Menu(Fl_Menu_Bar *menu_bar, const char *label);
   virtual ~Menu();
   Menu *add(const char *label, const char *shortcut, menu_callback_t callback, int flags=0, int group=0, xpm::xpm_t icon=xpm::no_image);
   void enable(bool enabled, int group = 0);
 private:
   Menu(int group, menu_callback_t callback);
   static void menu_action(Fl_Widget *widget, void *user_data);

 private:
    std::string path_;
    int index_;
    Fl_Menu_Bar *menu_bar_;
    int group_;
    menu_callback_t callback_;
    std::list<std::shared_ptr<Menu> > items_;
    std::shared_ptr<Fl_RGB_Image> img_;
    std::string label_text_;
    Fl_Multi_Label multi_label_;
};


}  // namespace vcutter

#endif  // SMART_VCUTTER_MENU_H

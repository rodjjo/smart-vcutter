/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_UI_CONTROLS_MENU_ITEM_H_
#define SRC_UI_CONTROLS_MENU_ITEM_H_

#include <memory>
#include <string>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Multi_Label.H>
#include "src/ui/action.h"
#include "src/data/xpm.h"


namespace vcutter {

class MenuBar2;

class MenuItem {
 public:
   MenuItem(
      MenuBar2* bar,
      const char *label,
      std::shared_ptr<Action> action,
      xpm::xpm_t icon=xpm::no_image);
   virtual ~MenuItem();
   void invalidate();

 private:
   static void callback(Fl_Widget *widget, void *user_data);

 private:
   std::shared_ptr<Action> action_;
   std::string label_;
   std::string path_;
   std::string shortcut_;
   std::shared_ptr<Fl_RGB_Image> img_;
   Fl_Multi_Label multi_label_;
   MenuBar2 *bar_;
};  // MenuItem

}  // namespace vcutter

#endif  // SRC_UI_CONTROLS_MENU_ITEM_H_

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_WND_CUTTER_OPTIONS_WINDOW_H_
#define SMART_VCUTTER_WND_CUTTER_OPTIONS_WINDOW_H_

#include <string>
#include <memory>
#include <set>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Image.H>


namespace vcutter {

class CutterOptionsWindow {
 public:
    CutterOptionsWindow();
    ~CutterOptionsWindow();
    static bool edit_properties(unsigned int max_w, unsigned int max_h, unsigned int *w, unsigned int *h);
  private:
    bool edit(unsigned int max_w, unsigned int max_h, unsigned int *w, unsigned int *h);
    static void button_callback(Fl_Widget* widget, void *userdata);
    static void edit_callback(Fl_Widget* widget, void *userdata);
    void action_confirm();
    void action_cancel();
    bool copy_number(const char* value, unsigned int *output);
    void sync_wh(const void *who);
    bool validate_inputs(bool show_errors);
  private:
    Fl_Window *window_;
    Fl_Group *components_group_;
    Fl_Group *buttons_group_;
    Fl_Input *edt_width_;
    Fl_Input *edt_height_;
    Fl_Button *btn_ok_;
    Fl_Button *btn_cancel_;
    Fl_Button *btn_lock_;
    Fl_Button *btn_unlock_;
  private:
    std::set<std::shared_ptr<Fl_Image> > images_;
    bool confirmed_;
    unsigned int lock_w_;
    unsigned int lock_h_;
    unsigned int max_w_;
    unsigned int max_h_;
};

}

#endif  // SMART_VCUTTER_WND_CUTTER_OPTIONS_WINDOW_H_
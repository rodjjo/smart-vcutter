/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_CONTROLS_BUTTON_H_
#define SRC_CONTROLS_BUTTON_H_

#include <memory>
#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>

#include "src/wnd_main/callbacks.h"

namespace vcutter {

class Button {
 public:
    Button(const char *label, callback_t callback);
    Button(std::shared_ptr<Fl_Image> image, callback_t callback);
    void change_label(const char *label);
    void position(int px, int py);
    void shortcut(int value);
    void size(int sw, int sh);
    void tooltip(const char *text);
    int x();
    int y();
    int w();
    int h();
 private:
    static void button_callback(Fl_Widget* widget, void *userdata);
 private:
    callback_t callback_;
    Fl_Button *button_;
    std::shared_ptr<Fl_Image> image_;
};

}  // namespace vcutter

#endif  // SRC_CONTROLS_BUTTON_H_

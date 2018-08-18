/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <strings.h>
#include <memory>
#include <Fl/fl_ask.H>
#include <Fl/Fl.H>
#include "src/wnd_cutter/options_window.h"
#include "src/data/xpm.h"


namespace vcutter {

namespace {
const int kWINDOW_WIDTH = 300;
const int kWINDOW_HEIGHT = 100;
}

CutterOptionsWindow::CutterOptionsWindow() {
    confirmed_ = false;
    max_w_ = 0;
    max_h_ = 0;
    lock_w_ = 0;
    lock_h_ = 0;
    window_ = new Fl_Window(0, 0, kWINDOW_WIDTH, kWINDOW_HEIGHT);
    window_->label("Output properties");
    components_group_ = new Fl_Group(0,0, window_->w(), window_->h() - 30);
    components_group_->box(FL_DOWN_BOX);
    edt_width_ = new Fl_Input(5,25, 130, 25, "Width:");
    edt_width_->align(FL_ALIGN_TOP_LEFT);
    btn_lock_ = new Fl_Button(edt_width_->x() + edt_width_->w() + 2, edt_width_->y(), 25, 25, "");
    btn_unlock_ = new Fl_Button(edt_width_->x() + edt_width_->w() + 2, edt_width_->y(), 25, 25, "");
    edt_height_ = new Fl_Input(btn_unlock_->x() + btn_unlock_->h() + 2, 25, edt_width_->w(), 25, "Height:");
    edt_height_->align(FL_ALIGN_TOP_LEFT);

    components_group_->end();

    buttons_group_ = new Fl_Group(0,0, window_->w(), 30);
    buttons_group_->box(FL_UP_BOX);
    btn_ok_ = new Fl_Button(window_->w() - 215, 3, 100, 23, "Ok");
    btn_cancel_ = new Fl_Button(window_->w() - 110, 3, 100, 23, "Cancel");
    buttons_group_->end();

    buttons_group_->position(0, window_->h() - 30);

    btn_ok_->callback(button_callback, this);
    btn_cancel_->callback(button_callback, this);
    btn_lock_->callback(button_callback, this);
    btn_unlock_->callback(button_callback, this);
    edt_width_->callback(edit_callback, this);
    edt_height_->callback(edit_callback, this);

    btn_unlock_->hide();

    btn_unlock_->clear_visible_focus();
    btn_lock_->clear_visible_focus();

    window_->end();
    window_->set_modal();

    auto img = xpm::image(xpm::lock_16x16);
    btn_lock_->image(img.get());
    images_.insert(img);
    img = xpm::image(xpm::unlock_16x16);
    btn_unlock_->image(img.get());
    images_.insert(img);

    btn_lock_->align(FL_ALIGN_IMAGE_BACKDROP);
    btn_unlock_->align(FL_ALIGN_IMAGE_BACKDROP);
}

CutterOptionsWindow::~CutterOptionsWindow() {
    Fl::delete_widget(window_);
    Fl::do_widget_deletion();
}

bool CutterOptionsWindow::edit_properties(unsigned int max_w, unsigned int max_h, unsigned int *w, unsigned int *h) {
    std::unique_ptr<CutterOptionsWindow> wnd(new CutterOptionsWindow());
    return wnd->edit(max_w, max_h, w, h);
}

bool CutterOptionsWindow::edit(
    unsigned int max_w, unsigned int max_h, unsigned int *w, unsigned int *h) {
    confirmed_ = false;
    char buffer[50] = "";
    snprintf(buffer, sizeof(buffer), "%u", *w);
    edt_width_->value(buffer);
    snprintf(buffer, sizeof(buffer), "%u", *h);
    edt_height_->value(buffer);
    max_w_ = max_w;
    max_h_ = max_h;
    lock_w_ = *w;
    lock_h_ = *h;

    window_->show();

    int left = (Fl::w() / 2) - window_->w() / 2;
    int top = (Fl::h() / 2) - window_->h() / 2;

    window_->position(left, top);

    while (window_->shown()) {
        Fl::wait();
    }

    if (confirmed_) {
        copy_number(edt_width_->value(), w);
        copy_number(edt_height_->value(), h);
    }

    return confirmed_;
}

bool CutterOptionsWindow::copy_number(const char* value, unsigned int *output) {
    return sscanf(value, "%u", output) == 1;
}


void CutterOptionsWindow::button_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<CutterOptionsWindow *>(userdata);
    if (widget == window->btn_ok_) {
        window->action_confirm();
    } else if (widget == window->btn_cancel_) {
        window->action_cancel();
    } else if (widget == window->btn_lock_) {
        window->btn_lock_->hide();
        window->btn_unlock_->show();
    } else if (widget == window->btn_unlock_) {
        if (window->validate_inputs(false)) {
            window->copy_number(window->edt_width_->value(), &window->lock_w_);
            window->copy_number(window->edt_height_->value(), &window->lock_h_);
            window->btn_lock_->show();
            window->btn_unlock_->hide();
        }
    }
}

void CutterOptionsWindow::sync_wh(const void *who) {
    unsigned int v;
    if (lock_w_ == 0 || lock_h_ == 0) {
        return;
    }
    float scale = lock_w_ / static_cast<float>(lock_h_);
    char buffer[25] = "";
    if (who == edt_width_) {
        copy_number(edt_width_->value(), &v);
        v = (v / scale) + 0.5;
        snprintf(buffer, sizeof(buffer) - 1, "%d", v);
        edt_height_->value(buffer);
    } else {
        copy_number(edt_height_->value(), &v);
        v = (v * scale) + 0.5;
        if (v % 2 != 0) {
            v += 1;
        }
        snprintf(buffer, sizeof(buffer) - 1, "%d", v);
        edt_width_->value(buffer);
    }
}

void CutterOptionsWindow::edit_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<CutterOptionsWindow *>(userdata);
    if (window->btn_lock_->visible()) {
        window->sync_wh(widget);
    }
}

bool CutterOptionsWindow::validate_inputs(bool show_errors) {
    unsigned int w, h;
    const char *error = NULL;
    char buffer[100];
    if (!copy_number(edt_width_->value(), &w)) {
        error = "You must type a valid width";
    } else if (w % 2) {
        error = "The width must be multiple of 2";
    } else if (!copy_number(edt_height_->value(), &h)) {
        error = "You must type a valid width";
    } else if (w > max_w_) {
        snprintf(buffer, sizeof(buffer), "The max allowed with is %u", max_w_);
        error = buffer;
    } else if (h > max_h_) {
        snprintf(buffer, sizeof(buffer), "The max allowed height is %u", max_h_);
        error = buffer;
    }
    if (error) {
        if (show_errors) {
            fl_alert("%s", error);
        }
        return false;
    }
    return true;;
}

void CutterOptionsWindow::action_confirm() {
    confirmed_ = validate_inputs(true);
    if (confirmed_) {
        window_->hide();
    }
}

void CutterOptionsWindow::action_cancel() {
    confirmed_ = false;
    window_->hide();
}

}  // namespace vcutter

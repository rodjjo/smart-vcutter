/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <Fl/Fl.H>

#include "smart-vcutter/wnd_common/common_dialogs.h"
#include  "smart-vcutter/wnd_common/progress_window.h"

namespace vcutter {

namespace {

const int kWINDOW_WIDTH = 400;
const int kWINDOW_HEIGHT = 500;
const int kWINDOW_NOVIDEO_HEIGHT = 100;

}

ProgressWindow::ProgressWindow(bool show_video) {
    has_progress_cb_ = false;
    init(show_video);
}

ProgressWindow::ProgressWindow(buffer_func_t buffer_func) {
    has_progress_cb_ = false;
    buffer_func_ = buffer_func;
    init(true);
}

ProgressWindow::ProgressWindow(progress_func_t progress_func) {
    has_progress_cb_ = true;
    progress_func_ = progress_func;
    init(false);
}

ProgressWindow::ProgressWindow(progress_func_t progress_func, buffer_func_t buffer_func) {
    has_progress_cb_ = true;
    progress_func_ = progress_func;
    buffer_func_ = buffer_func;
    init(true);
}

void ProgressWindow::init(bool with_video) {
    buffer_ = NULL;
    buffer_w_ = 0;
    buffer_h_ = 0;
    canceled_ = false;
    int window_height = with_video ? kWINDOW_HEIGHT : kWINDOW_NOVIDEO_HEIGHT;
    window_ = new Fl_Window(0, 0, kWINDOW_WIDTH, window_height);
    window_->color(fl_rgb_color(200, 200, 255));
    window_->label("Wait the task completion");

    bottom_group_ = new Fl_Group(0, window_->h() - 37, window_->w(), 37, "");
    bottom_group_->box(FL_DOWN_BOX);
    btn_cancel_ = new Fl_Button(window_->w() / 2 - 50, window_->h() - 27, 100, 23, "Cancel");
    bottom_group_->end();

    progress_label_ = new Fl_Box(FL_DOWN_BOX, window_->w() / 2 - 141, bottom_group_->y() - 30, 140, 25, "Progress:");
    progress_label_->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);
    progress_value_ = new Fl_Box(FL_DOWN_BOX, progress_label_->x() + 2 + progress_label_->w(), progress_label_->y(), progress_label_->w(), 25, "");
    progress_value_->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);

    if (with_video) {
        video_window_ = new BufferViewer(this, 5, 32, kWINDOW_WIDTH - 10,  progress_label_->y() - 40);
        btn_video_ = new Fl_Check_Button(5, video_window_->y() - 35, 25, 25, "Display Video (may slow down)");
        btn_video_->value(1);
    } else {
        video_window_ = NULL;
        btn_video_ = NULL;
    }

    btn_cancel_->callback(handle_cancel_action, this);
    window_->callback(handle_cancel_action, this);

    window_->end();

    window_->set_modal();


    reset_progress();
}

ProgressWindow::~ProgressWindow() {
    Fl::delete_widget(window_);
    Fl::do_widget_deletion();
}

void ProgressWindow::reset_progress() {
    canceled_ = false;
    asking_cancelation_ = false;
    progress_ = 0;
    last_progress_ = -1;
    max_progress_ = 100;
    last_perc_ = 0;
}

bool ProgressWindow::wait() {
    prepare_for_show();

    while (window_->shown()) {
        Fl::wait();
    }

    unprepare_for_show();

    return !canceled_;
 }

 void ProgressWindow::prepare_for_show() {
    reset_progress();

    window_->position(Fl::w() / 2 - window_->w() / 2, Fl::h() / 2 - window_->h() / 2);

    Fl::add_timeout(0.101, &ProgressWindow::timeout_handler, this);

    window_->show();

    buffer_ = NULL;
    buffer_w_ = 0;
    buffer_h_ = 0;
 }

 void ProgressWindow::unprepare_for_show() {
    Fl::remove_timeout(&ProgressWindow::timeout_handler, this);
    buffer_ = NULL;
    buffer_w_ = 0;
    buffer_h_ = 0;
 }

bool ProgressWindow::wait(progress_task_t task) {
    prepare_for_show();

    bool result = task();

    unprepare_for_show();

    return result;
 }

void ProgressWindow::cancel(bool confirm) {
    asking_cancelation_ = true;
    if (confirm && ask("Do you want to cancel ?"))  {
        update_progress();
        canceled_ = progress_ < max_progress_;
        window_->hide();
    }
    asking_cancelation_ = false;
}

bool ProgressWindow::should_display_video() {
    if (video_window_ == NULL || btn_video_->value() == 0) {
        return false;
    }

    return true;
}

void ProgressWindow::update_progress() {
    if (has_progress_cb_) {
        progress_func_(&progress_, &max_progress_);
    }
}

void ProgressWindow::set_progress(int64_t progress, int64_t max_progress) {
    progress_ = progress;
    max_progress_ = max_progress;
}

void ProgressWindow::display_progress() {
    update_progress();

    if (should_display_video()) {
        video_window_->redraw();
    }

    float perc = last_perc_;
    if (max_progress_) {
        perc = static_cast<int>(((100.0 / max_progress_) * progress_) * 100.00) / 100.0;
    }

    if (last_perc_ != perc) {
        last_perc_ = perc;
        snprintf(progress_label_buffer_, sizeof(progress_label_buffer_), "%0.02f %%", last_perc_);
        progress_value_->label(progress_label_buffer_);
    }

    last_progress_ = progress_;
 }

 void ProgressWindow::timeout_handler(void* data) {
    static_cast<ProgressWindow *>(data)->display_progress();

    Fl::repeat_timeout(0.101, ProgressWindow::timeout_handler, data);
 }

void ProgressWindow::handle_cancel_action(Fl_Widget *widget, void *this_window) {
    static_cast<ProgressWindow *>(this_window)->cancel(true);
}

void ProgressWindow::viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) {
    if (btn_video_->value() == 0) {
        return;
    }
    if (buffer_func_) {
        buffer_func_(buffer, w, h);
    } else {
        *buffer = buffer_;
        *w = buffer_w_;
        *h = buffer_h_;
    }
}

void ProgressWindow::set_buffer(const unsigned char *buffer, int w, int h) {
    buffer_ = buffer;
    buffer_w_ = w;
    buffer_h_ = h;
}

bool ProgressWindow::canceled() {
    return canceled_;
}

}   // namespace vcutter
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_WND_COMMON_PROGRESS_WINDOW_H_
#define SRC_WND_COMMON_PROGRESS_WINDOW_H_

#include <inttypes.h>
#include <functional>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include "src/clippings/clipping_conversion.h"
#include "src/viewer/buffer_viewer.h"


namespace vcutter {

typedef std::function<void(int64_t* progress, int64_t* max_progess)> progress_func_t;
typedef std::function<void(const unsigned char **buffer, uint32_t *w, uint32_t *h)> buffer_func_t;

class ProgressWindow: public BufferSupplier, public ProgressHandler {
 public:
    explicit ProgressWindow(bool show_video=false);
    ProgressWindow(buffer_func_t buffer_func);
    ProgressWindow(progress_func_t progress_func);
    ProgressWindow(progress_func_t progress_func, buffer_func_t buffer_func);
    virtual ~ProgressWindow();
    void set_progress(uint32_t progress, uint32_t max_progress) override;
    bool wait(progress_task_t task) override;
    bool wait();
    void cancel(bool confirm=false);
    bool canceled() override;
    void set_buffer(uint8_t *buffer, uint32_t w, uint32_t h) override;
 private:
    static void handle_cancel_action(Fl_Widget *widget, void *this_window);
    static void timeout_handler(void* data);
    void prepare_for_show();
    void unprepare_for_show();
    void update_progress();
    void display_progress();
    bool should_display_video();
    void viewer_buffer(BufferViewer *viewer, const unsigned char** buffer, uint32_t *w, uint32_t *h) override;
    void init(bool with_video);
    void reset_progress();
 private:
    bool canceled_;
    bool asking_cancelation_;
    bool has_progress_cb_;
    char progress_label_buffer_[20];
    Fl_Group *bottom_group_;
    Fl_Window *window_;
    Fl_Box *progress_label_;
    Fl_Box *progress_value_;
    Fl_Check_Button *btn_video_;
    Fl_Button *btn_cancel_;
    BufferViewer *video_window_;
    progress_func_t progress_func_;
    buffer_func_t buffer_func_;
    float last_perc_;
    int64_t progress_;
    int64_t last_progress_;
    int64_t max_progress_;
    const unsigned char *buffer_;
    int buffer_w_;
    int buffer_h_;

};



}  // namespace vcutter

#endif  // SRC_WND_COMMON_PROGRESS_WINDOW_H_

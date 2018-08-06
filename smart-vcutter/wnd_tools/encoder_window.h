/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_WND_TOOLS_ENCODER_WINDOW_H
#define SMART_VCUTTER_WND_TOOLS_ENCODER_WINDOW_H

#include <memory>
#include <atomic>
#include <string>
#include <map>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>

#include "smart-vcutter/data/project.h"
#include "video_stream/vstream/video_stream.h"
#include "smart-vcutter/data/session.h"
#include "smart-vcutter/data/history.h"

namespace vcutter {

class EncoderWindow {
 public:
    explicit EncoderWindow(History* history);
    EncoderWindow(History* history, const std::string & path);
    EncoderWindow(History* history, const clipping_t & clip);
    virtual ~EncoderWindow();
    static void execute(History* history, Fl_Window *parent);
    static void execute(History* history, Fl_Window *parent, const clipping_t & clip);
    static void execute(History* history, Fl_Window *parent, const std::string& path);
    static void restore_session(History* history, Fl_Window *parent);

 private:
    std::map<std::string,std::string> serialize();
    bool deserialize(const session_data_t & data);

    void init(History* history, const clipping_t * clip);
    void show_modal(Fl_Window *parent);
    void fill_animation_info(int video_frame_count);
    void copy_original_fps();
    static void button_callback(Fl_Widget* widget, void *userdata);
    static void update_bitrate_cb(Fl_Widget* widget, void *userdata);
    static void update_filesize_cb(Fl_Widget* widget, void *userdata);
    void adapt_ui();
    void action_output();
    void action_fps();
    void action_convert();
    void action_video_path();
    void action_close();
    void update_bitrate();
    void update_filesize();
    double choosen_fps();
    uint32_t choosen_bitrate();
    double calc_fps();
    double calc_duration();
    int64_t calc_filesize();
    uint32_t calc_bitrate();
    void save_sugestion();
    std::string get_sugestion();
    void sugest_output_file();
    const char *sugest_extension();
 private:
    clipping_t clip_;
    bool has_clipping_;
    int frame_w_;
    int frame_h_;
    std::string path_;
    std::string last_filepath_sug_;
    double original_fps_;
    static std::string last_filepath_;
    static std::string last_sugestion_;
 private:
    const void* bitrate_action_src_;
    History* history_;
    Fl_Window *window_;
    Fl_Group *components_group_;
    Fl_Group *buttons_group_;
    Fl_Input *edt_path_;
    Fl_Input *edt_output_;
    Fl_Choice *cmb_formats_;
    Fl_Float_Input *edt_bitrate_;
    Fl_Float_Input *edt_fps_;
    Fl_Button *btn_bit_;
    Fl_Box *box_file_size_;
    Fl_Check_Button *btn_start_backward_;
    Fl_Check_Button *btn_append_reverse_;
    Fl_Check_Button *btn_merge_;
    Fl_Input *edt_start_;
    Fl_Input *edt_end_;
    Fl_Button *btn_fps_;
    Fl_Button *btn_path_;
    Fl_Button *btn_output_;
    Fl_Button *btn_convert_;
    Fl_Button *btn_close_;
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_WND_TOOLS_ENCODER_WINDOW_H
/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */

#include <regex> // NOLINT
#include <Fl/Fl.H>

#include "src/common/utils.h"
#include "src/wrappers/video_conversion.h"
#include "src/wnd_common/common_dialogs.h"
#include "src/wnd_tools/encoder_window.h"

namespace vcutter {

namespace {

const char *kSOURCE_DIR_KEY = "ews-source-dir";
const char *kCLIPPING_DIR_KEY = "ews-clipping-dir";
const char *kCONVERSION_DIR_KEY = "ews-conversion-dir";

const int kWINDOW_WIDTH = 700;
const int kWINDOW_HEIGHT = 300;

const char *kEDT_PATH_FIELD = "source_path";
const char *kEDT_OUTPUT_FIELD = "target_path";
const char *kCMB_FORMATS_FIELD = "formats";
const char *kEDT_BITRATE_FIELD = "bitrate";
const char *kEDT_FPS_FIELD = "fps";
const char *kCHE_BACKWARD_FIELD = "backward";
const char *kCHE_REVERSE_FIELD = "reverse";
const char *kCHE_MERGE_FIELD = "merge";
const char *kEDT_START_FIELD = "start";
const char *kEDT_END_FIELD = "end";
const char *kORI_FPS_FIELD = "original_fps";
const char *kCLIP_VAR_NAME = "clipping";
const char *kPATH_VAR_NAME = "path";

}  // namespace

std::string EncoderWindow::last_filepath_; // NOLINT
std::string EncoderWindow::last_sugestion_; // NOLINT

EncoderWindow::EncoderWindow(History *history, std::shared_ptr<Clipping> clip) {
    init(history, clip);
}

EncoderWindow::EncoderWindow(History* history) {
    init(history, NULL);
}

EncoderWindow::EncoderWindow(History* history, const std::string& path) {
    init(history, NULL);
    path_ = path;
}


void EncoderWindow::init(History* history, std::shared_ptr<Clipping> clip) {
    history_ = history;
    clip_ = clip;

    bitrate_action_src_ = NULL;
    frame_w_ = 0;
    frame_h_ = 0;
    original_fps_ = 0;

    window_ = new Fl_Window(0, 0, kWINDOW_WIDTH, kWINDOW_HEIGHT, clip_ ? "Generate animation" : "Convert video track (NO SOUND IN OUTPUT)");
    window_->begin();

    components_group_ = new Fl_Group(0,0, window_->w(), window_->h() - 30);
    components_group_->box(FL_DOWN_BOX);

    edt_path_ = new Fl_Input(5,25, window_->w() - 37, 25, clip_ ? "Animation from:" : "Source video path:");
    edt_path_->align(FL_ALIGN_TOP_LEFT);
    edt_path_->readonly(true);

    btn_path_ = new Fl_Button(edt_path_->x() + edt_path_->w() + 3, edt_path_->y(), 25, 25, "@fileopen");

    edt_start_ =  new Fl_Input(5,75, window_->w() * 0.5 - 2, 25, "Start Time:");
    edt_start_->align(FL_ALIGN_TOP_LEFT);

    edt_end_ = new Fl_Input(edt_start_->x() + window_->w() * 0.5 + 5, 75, edt_start_->w() - 15, 25, "End Time:");
    edt_end_->align(FL_ALIGN_TOP_LEFT);

    btn_start_backward_ = new Fl_Check_Button(edt_start_->x(), edt_end_->y() + edt_end_->h() + 1, window_->w() * 0.3, 25, "Start at the end");
    btn_append_reverse_ = new Fl_Check_Button(btn_start_backward_->x() + btn_start_backward_->w() + 5, edt_end_->y() + edt_end_->h() + 1, window_->w() * 0.3, 25, "Append a reverse copy");
    btn_merge_ = new Fl_Check_Button(btn_append_reverse_->x() + btn_append_reverse_->w() + 5, edt_end_->y() + edt_end_->h() + 1, window_->w() * 0.3, 25, "Combine last and first frames");

    edt_output_ = new Fl_Input(5, btn_append_reverse_->y() + 20 + btn_append_reverse_->h(), window_->w() - 37, 25, "Output path:");
    edt_output_->align(FL_ALIGN_TOP_LEFT);
    // edt_output_->readonly(true);

    btn_output_ = new Fl_Button(edt_output_->x() + edt_output_->w() + 3, edt_output_->y(), 25, 25, "@filenew");

    cmb_formats_ = new Fl_Choice(5, edt_output_->y() + edt_output_->h() + 25, 200, 25, "Format:");
    cmb_formats_->align(FL_ALIGN_TOP_LEFT);

    edt_bitrate_ =  new Fl_Float_Input(cmb_formats_->w() + 10, cmb_formats_->y(), 200, 25, "Bitrate (Mbits/s):" );
    edt_bitrate_->align(FL_ALIGN_TOP_LEFT);

    btn_bit_ = new Fl_Button(edt_bitrate_->x() + edt_bitrate_->w() + 1,  edt_bitrate_->y(), 50, 25, "Calc.");
    edt_fps_ = new Fl_Float_Input(btn_bit_->x() + btn_bit_->w() + 5, btn_bit_->y(), 200 - 61, 25, "Fps:");
    edt_fps_->align(FL_ALIGN_TOP_LEFT);

    btn_fps_ = new Fl_Button(edt_fps_->x() + edt_fps_->w() + 1,  edt_fps_->y(), 60, 25, "change");

    components_group_->end();

    buttons_group_ = new Fl_Group(0,0, window_->w(), 30);
    buttons_group_->box(FL_UP_BOX);
    btn_close_ = new Fl_Button(window_->w() - 110, 3, 100, 23, "Close");
    btn_convert_ = new Fl_Button(btn_close_->x() - 10 - btn_close_->w(), 3, 100, 23, "Convert");
    box_file_size_ = new Fl_Box(FL_DOWN_BOX, 5, 3, btn_convert_->x() - 10, 23, "");
    box_file_size_->align(FL_ALIGN_INSIDE | FL_ALIGN_RIGHT);

    buttons_group_->end();

    buttons_group_->position(0, window_->h() - 30);

    window_->end();

    window_->set_modal();

    btn_output_->tooltip("Choose the output file");
    btn_path_->tooltip("Choose the file to convert");

    const char** formats = vs::Encoder::format_names();
    int webm_index = 0;
    int index = 0;
    while (*formats) {
        cmb_formats_->add(*formats);
        if (strcmp(*formats, "mp4-x264") == 0) {
            webm_index = index;
        }
        ++formats;
        ++index;
    }

    cmb_formats_->value(webm_index);

    btn_close_->callback(button_callback, this);
    btn_path_->callback(button_callback, this);
    btn_output_->callback(button_callback, this);
    btn_convert_->callback(button_callback, this);
    btn_fps_->callback(button_callback, this);

    btn_bit_->callback(update_bitrate_cb, this);
    edt_fps_->callback(update_bitrate_cb, this);
    cmb_formats_->callback(update_bitrate_cb, this);
    edt_start_->callback(update_bitrate_cb, this);
    edt_end_->callback(update_bitrate_cb, this);

    btn_append_reverse_->callback(update_bitrate_cb, this);

    edt_bitrate_->callback(update_filesize_cb, this);

    adapt_ui();
}

EncoderWindow::~EncoderWindow() {
    Fl::delete_widget(window_);
    Fl::do_widget_deletion();
}

void EncoderWindow::adapt_ui() {
    if (clip_) {
        btn_path_->hide();
        edt_start_->readonly(true);
        edt_end_->readonly(true);
        edt_start_->color(fl_rgb_color(200, 200, 200));
        edt_end_->color(fl_rgb_color(200, 200, 200));
        edt_path_->color(fl_rgb_color(200, 200, 200));
    } else {
        btn_merge_->hide();
    }
}

void EncoderWindow::execute(History* history, Fl_Window *parent) {
    std::unique_ptr<EncoderWindow> window(new EncoderWindow(history));
    window->show_modal(parent);
}


std::map<std::string, std::string> EncoderWindow::serialize() {
    std::map<std::string, std::string> result;
    result[kEDT_PATH_FIELD] = edt_path_->value();
    result[kEDT_OUTPUT_FIELD] = edt_output_->value();
    result[kCMB_FORMATS_FIELD] = cmb_formats_->text();
    result[kEDT_BITRATE_FIELD] = edt_bitrate_->value();
    result[kEDT_FPS_FIELD] = edt_fps_->value();
    result[kCHE_BACKWARD_FIELD] = btn_start_backward_->value() ? "yes" : "no";
    result[kCHE_REVERSE_FIELD] = btn_append_reverse_->value() ? "yes" : "no";
    result[kCHE_MERGE_FIELD] = btn_merge_->value() ? "yes" : "no";
    result[kEDT_START_FIELD] = edt_start_->value();
    result[kEDT_END_FIELD] = edt_end_->value();

    char tmp[32] = "";
    snprintf(tmp, sizeof(tmp), "%lf", original_fps_);
    result[kORI_FPS_FIELD] = tmp;

    if (clip_) {
        Json::Value root = clip_->serialize();
        result[kCLIP_VAR_NAME] = Json::FastWriter().write(root);
    }

    return result;
}

bool EncoderWindow::deserialize(const string_map_t & data) {
    auto path_param = data.find(kEDT_PATH_FIELD);
    auto clip_param = data.find(kCLIP_VAR_NAME);

    if (path_param == data.end() && clip_param == data.end()) {
        return false;
    }

    if (clip_param == data.end()) {
        if (path_param->second.empty()) {
            return false;
        }
    }

    if (data.find(kEDT_PATH_FIELD) != data.end())
        edt_path_->value(data.at(kEDT_PATH_FIELD).c_str());

    if (data.find(kEDT_OUTPUT_FIELD) != data.end())
        edt_output_->value(data.at(kEDT_OUTPUT_FIELD).c_str());

    if (data.find(kCMB_FORMATS_FIELD) != data.end()) {
        for (int i = 0; i < cmb_formats_->size(); ++i) {
            if (strcmp(cmb_formats_->menu()[i].label(), data.at(kCMB_FORMATS_FIELD).c_str()))
                continue;
            cmb_formats_->value(i);
            break;
        }
    }

    if (data.find(kEDT_BITRATE_FIELD) != data.end())
        edt_bitrate_->value(data.at(kEDT_BITRATE_FIELD).c_str());

    if (data.find(kEDT_FPS_FIELD) != data.end())
        edt_fps_->value(data.at(kEDT_FPS_FIELD).c_str());

    if (data.find(kCHE_BACKWARD_FIELD) != data.end())
        btn_start_backward_->value(data.at(kCHE_BACKWARD_FIELD) == "yes");

    if (data.find(kCHE_REVERSE_FIELD) != data.end())
        btn_append_reverse_->value(data.at(kCHE_REVERSE_FIELD) == "yes");
    if (data.find(kCHE_MERGE_FIELD) != data.end())
        btn_merge_->value(data.at(kCHE_MERGE_FIELD) == "yes");

    if (data.find(kEDT_START_FIELD) != data.end())
        edt_start_->value(data.at(kEDT_START_FIELD).c_str());

    if (data.find(kEDT_END_FIELD) != data.end())
        edt_end_->value(data.at(kEDT_END_FIELD).c_str());

    if (data.find(kPATH_VAR_NAME) != data.end())
        path_ = data.at(kPATH_VAR_NAME);

    original_fps_ = 24;

    if (data.find(kORI_FPS_FIELD) != data.end()) {
        sscanf(data.at(kORI_FPS_FIELD).c_str(), "%lf", &original_fps_);
    }

    if (clip_param != data.end()) {
        path_.clear();

        Json::Value clipping_data;

        if (!Json::Reader().parse(data.at(kCLIP_VAR_NAME), clipping_data, false)) {
            return false;
        }

        clip_.reset(new Clipping(&clipping_data));

        if (!clip_->good()) {
            clip_.reset();
            return false;
        }
    }

    adapt_ui();

    return true;
}

void EncoderWindow::execute(History* history, Fl_Window *parent, std::shared_ptr<Clipping> clip) {
    if (clip->video_path().empty()) {
        show_error("The clip must define a path to de video");
        return;
    }

    if (static_cast<int>(clip->w()) % 2) {
        show_error("Invalid clip width. The width must be multiple of 2");
        return;
    }

    std::unique_ptr<EncoderWindow> window(new EncoderWindow(history, clip));
    window->show_modal(parent);
}


void EncoderWindow::restore_session(History* history, Fl_Window *parent) {
    JsonFile session(temp_filepath("vcutter-recovery-ews.json").c_str(), true, true);

    if (!session.loaded()) {
        return;
    }

    std::unique_ptr<EncoderWindow> window(new EncoderWindow(history, ""));

    const char *keys[] = {
        kEDT_PATH_FIELD,
        kEDT_OUTPUT_FIELD,
        kCMB_FORMATS_FIELD,
        kEDT_BITRATE_FIELD,
        kEDT_FPS_FIELD,
        kCHE_BACKWARD_FIELD,
        kCHE_REVERSE_FIELD,
        kCHE_MERGE_FIELD,
        kEDT_START_FIELD,
        kEDT_END_FIELD,
        kORI_FPS_FIELD,
        kCLIP_VAR_NAME,
        kPATH_VAR_NAME,
        NULL
    };

    Json::Value root = session["recovery"];
    string_map_t data;

    const char *key = NULL;
    for(int index = 0; ; ++index) {
        key = keys[index];
        if (!key)
            break;
        if (root.isMember(key)) {
            data[key] = root[key].asString();
        }
    }

    if (!window->deserialize(data)) {
        return;
    }

    window->show_modal(parent);
}


void EncoderWindow::execute(History* history, Fl_Window *parent, const std::string& path) {
    if (path.empty()) {
        show_error("Invalid file path.");
        return;
    }

    std::unique_ptr<EncoderWindow> window(new EncoderWindow(history, path));
    window->show_modal(parent);
}

void EncoderWindow::show_modal(Fl_Window *parent) {
    window_->position(
        (parent->x() + parent->w() / 2) - window_->w() / 2,
        (parent->y() + parent->h() / 2) - window_->h() / 2);

    window_->show();

    if (clip_ || !path_.empty()) {
        action_video_path();
    }

    while (window_->shown()) {
        Fl::wait();
    }
}

void EncoderWindow::button_callback(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<EncoderWindow *>(userdata);

    if (widget == window->btn_close_) {
        window->action_close();
    } else if (widget == window->btn_path_) {
        window->action_video_path();
    } else if (widget == window->btn_output_) {
        window->action_output();
    } else if (widget == window->btn_convert_) {
        window->action_convert();
    } else if (widget == window->btn_fps_) {
        window->action_fps();
    }
}

void EncoderWindow::action_fps() {
    std::string fps = edt_fps_->value();
    bitrate_action_src_ = edt_fps_;
    if (fps == "15") {
        edt_fps_->value("18");
    } else if (fps == "18") {
        edt_fps_->value("20");
    } else if (fps == "20") {
        edt_fps_->value("24");
    } else if (fps == "24") {
        copy_original_fps();
    } else {
        edt_fps_->value("15");
    }
    update_bitrate();
    bitrate_action_src_ = NULL;
}

void EncoderWindow::action_convert() {
    if (!strlen(edt_path_->value())) {
        show_error("You must specify the source video path");
        return;
    }

    if (!filepath_exists(edt_path_->value())) {
        show_error("The source video file does not exist");
        return;
    }

    int start_time = 0;
    int end_time = -1;
    if (strlen(edt_start_->value())) {
        start_time = str_to_milliseconds(edt_start_->value());
        if (start_time == 0 && strcmp(edt_start_->value(), "00:00:00,000") != 0) {
            show_error("Invalid time format for the start time");
            return;
        }
    }

    if (strlen(edt_end_->value())) {
        end_time = str_to_milliseconds(edt_end_->value());
        if (end_time == 0 && strcmp(edt_end_->value(), "00:00:00,000") != 0) {
            show_error("Invalid time format for the end time");
            return;
        }
        if (end_time <= start_time) {
            show_error("The end time must be bigger than the start time");
            return;
        }
    }

    if (choosen_bitrate() == 0) {
        show_error("You must specify a bitrate bigger than 0");
        return;
    }

    if (choosen_fps() <= 0) {
        show_error("The frame rate must be bigger than 0.000 fps");
        return;
    }

    if (!strlen(edt_output_->value())) {
        action_output();
        if (!strlen(edt_output_->value())) {
            return;
        }
    }

    int start_frame = static_cast<int>((start_time / 1000.0f) * original_fps_ + 0.5);
    int end_frame = static_cast<int>((end_time / 1000.0f) * original_fps_ + 0.5);

    if (btn_start_backward_->value()) {
        start_frame = start_frame ^ end_frame;
        end_frame = start_frame ^ end_frame;
        start_frame = start_frame ^ end_frame;
    }

    const char *format = cmb_formats_->text();

    JsonFile session(temp_filepath("vcutter-recovery-ews.json").c_str(), true, false);

    string_map_t data = serialize();

    Json::Value recovery_data;
    for (auto it = data.begin(); it != data.end(); ++it) {
        recovery_data[it->first] = it->second;
    }

    session["recovery"] = recovery_data;

    session.save();

    if (clip_) {
        VideoConversionWrapper converter(
            clip_,
            format,
            edt_output_->value(),
            choosen_bitrate(),
            choosen_fps(),
            btn_start_backward_->value() != 0
        );

        converter.convert(btn_append_reverse_->value() != 0, btn_merge_->value() != 0);
        save_sugestion();
    } else {
        VideoConversionWrapper converter(
            edt_path_->value(),
            start_frame,
            end_frame,
            format,
            edt_output_->value(),
            choosen_bitrate(),
            choosen_fps());

        converter.convert(btn_append_reverse_->value() != 0, btn_merge_->value() != 0);
    }
}

double EncoderWindow::choosen_fps() {
    double fps = 0;
    sscanf(edt_fps_->value(), "%lf", &fps);
    return fps;
}

uint32_t EncoderWindow::choosen_bitrate() {
    float bits = 0;
    sscanf(edt_bitrate_->value(), "%f", &bits);
    return bits * 1048576;
}

const char *EncoderWindow::sugest_extension() {
    return  strcmp(cmb_formats_->text(), "webm") ? ".mp4" : ".webm";
}

void EncoderWindow::action_output() {
    const char *key = clip_ ? kCLIPPING_DIR_KEY : kCONVERSION_DIR_KEY;
    std::string directory = (*history_)[key];

    std::string path_to_save =
        strcmp(cmb_formats_->text(), "webm") ?
        output_mjpeg_file_chooser(&directory, sugest_extension()) :
        output_webm_file_chooser(&directory, sugest_extension());

    if (!directory.empty()) {
        history_->set(key, directory.c_str());
    }

    if (path_to_save.empty()) {
         return;
    }

    edt_output_->value(path_to_save.c_str());
}

double EncoderWindow::calc_fps() {
    return original_fps_;
}

double EncoderWindow::calc_duration() {
    double duration = 0;

    double start_time = str_to_milliseconds(edt_start_->value());
    double end_time = str_to_milliseconds(edt_end_->value());

    if (end_time > start_time) {
        duration = end_time - start_time;
    }

    if (btn_append_reverse_->value()) {
        duration *= 2;
    }

    double fps = choosen_fps();

    if (!fps) {
        duration = 0;
    } else {
        duration *=  (original_fps_ / fps);
    }

    return duration / 1000.0;
}

int64_t EncoderWindow::calc_filesize() {
    double dur = calc_duration();
    return dur * (choosen_bitrate() / 8.0);
}

uint32_t EncoderWindow::calc_bitrate() {
    return vs::Encoder::default_bitrate(cmb_formats_->text(), frame_w_, frame_h_, choosen_fps());
}

void EncoderWindow::update_bitrate_cb(Fl_Widget* widget, void *userdata) {
    auto window = static_cast<EncoderWindow *>(userdata);
    window->bitrate_action_src_ = widget;
    window->update_bitrate();
    window->bitrate_action_src_ = NULL;

    if (!window->clip_ || window->btn_append_reverse_->value() != 0) {
        window->btn_merge_->hide();
    } else if (window->clip_) {
        window->btn_merge_->show();
    }
}

void EncoderWindow::update_filesize_cb(Fl_Widget* widget, void *userdata) {
    static_cast<EncoderWindow *>(userdata)->update_filesize();
}

void EncoderWindow::update_bitrate() {
    if (bitrate_action_src_ != btn_bit_
        && bitrate_action_src_ != cmb_formats_
        && bitrate_action_src_ != edt_fps_
        && strlen(edt_bitrate_->value())) {
        update_filesize();
        return;
    }
    uint32_t bitrate = calc_bitrate();
    char buffer[50] = "";
    snprintf(buffer, sizeof(buffer) - 1, "%.3f", bitrate / 1048576.0f);
    edt_bitrate_->value(buffer);
    update_filesize();
}

void EncoderWindow::update_filesize() {
    double size = calc_filesize() / 1024.0;
    const char *label = "KB";

    if (size > 999) {
        size /= 1024.0;
        label = "MB";
    }

    if (size > 999) {
        size /=  1024.0;
        label = "GB";
    }

    char buffer[200] = "";

    snprintf(buffer, sizeof(buffer) - 1,
        "[%.2lf segs] Output file size estimated in: %0.1lf %s", calc_duration(),  size, label);
    box_file_size_->copy_label(buffer);
}

void EncoderWindow::copy_original_fps() {
    if (original_fps_ != 0) {
        char temp[50] = "";
        snprintf(temp, sizeof(temp), "%0.3lf", original_fps_);
        edt_fps_->value(temp);
    } else {
        edt_fps_->value("");
    }
}

void EncoderWindow::fill_animation_info(int video_frame_count) {
    char temp[50] = "";

    double start_time = (clip_->first_frame() / original_fps_) - 1;
    double end_time = (clip_->last_frame() / original_fps_) - 1;

    if (end_time == start_time) {
        end_time = (video_frame_count / original_fps_) - 1;
    }

    if (start_time < 0) {
        start_time = 0;
    }

    if (end_time < 0) {
        end_time = 0;
    }

    seconds_to_str(temp, sizeof(temp), start_time, true);
    edt_start_->value(temp);
    seconds_to_str(temp, sizeof(temp), end_time, true);
    edt_end_->value(temp);
    frame_w_ = clip_->w();
    frame_h_ = clip_->h();
}

void EncoderWindow::sugest_output_file() {
    if (!strlen(edt_path_->value())) {
        return;
    }

    if (!clip_ && last_filepath_sug_ == edt_path_->value()) {
        edt_output_->value("");
    }

    if (strlen(edt_output_->value())) {
        return;
    }

    if (clip_ && !strlen(edt_output_->value())) {
        edt_output_->value(get_sugestion().c_str());
        if (strlen(edt_output_->value())) {
            return;
        }
    }

    std::string dir = (*history_)[clip_ ? kCLIPPING_DIR_KEY : kCONVERSION_DIR_KEY];
    if (dir.empty() || !filepath_exists(dir.c_str())) {
        return;
    }

    edt_output_->value(
        change_filepath_dir(edt_path_->value(), dir.c_str(), sugest_extension(), clip_.get() != NULL).c_str());
    if (clip_) {
        last_filepath_sug_ = edt_path_->value();
    }
}

void EncoderWindow::action_video_path() {
    std::string path;
    if (clip_) {
        path = clip_->video_path();
        btn_merge_->show();
    } else if (!path_.empty()) {
        path = path_;
        path_.clear();
    } else {
        std::string directory = (*history_)[kSOURCE_DIR_KEY];
        path = input_video_file_chooser(&directory);
        if (!directory.empty()) {
            history_->set(kSOURCE_DIR_KEY, directory.c_str());
        }
        if (!path.empty()) {
            edt_fps_->value("");
        }
    }

    if (path.empty()) {
         return;
    }

    edt_path_->value(path.c_str());

    edt_start_->value("");
    edt_end_->value("");

    original_fps_ = 0;

    frame_w_ = 0;
    frame_h_ = 0;

    auto player = vs::open_file(edt_path_->value());

    if (player->error()) {
        show_error(player->error());
        edt_path_->value("");
        if (clip_) {
            window_->hide();
        }
        return;
    }

    original_fps_ = player->fps();

    if (!original_fps_) {
        original_fps_ = 0.000001;
        edt_path_->value("");
        show_error("Could not retrive video's fps.");
        if (clip_) {
            window_->hide();
        }
        return;
    }

    if (strlen(edt_fps_->value()) == 0) {
        copy_original_fps();
    }

    if (clip_) {
        fill_animation_info(player->count());
    } else {
        char temp[50] = "";
        seconds_to_str(temp, sizeof(temp), player->duration(), true);
        edt_start_->value("00:00:00,000");
        edt_end_->value(temp);
        frame_w_ = player->w();
        frame_h_ = player->h();
    }

    update_bitrate();
    sugest_output_file();
}

void EncoderWindow::action_close() {
    window_->hide();
}

void EncoderWindow::save_sugestion() {
    last_filepath_ = edt_path_->value();
    last_sugestion_ = edt_output_->value();
}

std::string EncoderWindow::get_sugestion() {
    if (edt_path_->value() != last_filepath_) {
        return std::string();
    }

    if (last_sugestion_.empty()) {
        last_sugestion_ = edt_path_->value();
    }

    std::regex index_regex("^(.*\\.)([0-9]+)(\\.[a-z0-9]{3,4})$", std::regex_constants::icase);
    std::regex extension_regex("^(.*)(\\.[a-z0-9]{3,4})$", std::regex_constants::icase);

    std::smatch math_1;
    if (std::regex_search(last_sugestion_, math_1, index_regex)) {
        int index;
        if (sscanf(math_1[2].str().c_str(), "%d", &index) != 1) {
            index = 1;
        } else {
            ++index;
        }
        return generate_path(math_1[1].str().c_str(), math_1[3].str().c_str(), index);
    }

    std::smatch math_2;
    if (std::regex_search(last_sugestion_, math_2, extension_regex)) {
        return generate_path(math_2[1].str().c_str(), math_2[2].str().c_str(), 1);
    }

    return std::string();
}


}  // namespace vcutter

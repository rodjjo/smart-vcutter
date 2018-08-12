/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <boost/filesystem.hpp>
#include <Fl/fl_ask.H>

#include "src/common/utils.h"
#include "src/wnd_common/common_dialogs.h"


namespace vcutter {

namespace {

const char *kOUTPUT_VIDEO_FILE_FILTER = "Video files\t*.{mp4,webm,mjpeg}\n";
const char *kINPUT_VIDEO_FILE_FILTER = "Video files\t*.{avi,mp4,mkv,mpeg,wmv,mov}\n";
const char *kINPUT_PROJECT_FILE_FILTER = "Open clipping project\t*.vcutter\n";
const char *kOUTPUT_PROJECT_FILE_FILTER = "Save clipping project\t*.vcutter\n";
const char *kOUTPUT_MJPEG_FILE_FILTER = "MJPEG Videos\t*.mp4\n";
const char *kOUTPUT_WEBM_FILE_FILTER = "WEBM Videos\t*.webm\n";
const char *kINPUT_VIDEO_FILE_TITLE = "Select a video to open";
const char *kINPUT_PROJECT_FILE_TITLE = "Select a project to open";
const char *kOUTPUT_PROJECT_FILE_TITLE = "Define a location to save the project";
const char *kOUTPUT_VIDEO_FILE_TITLE = "Define a location to save the video";

}  // namespace


bool ask(const char *message) {
    return fl_choice(message, "No", "Yes", NULL) == 1;
}

int yes_nc(const char *message) {
    return 2 - fl_choice(message, "No", "Cancel", "Yes");
}

void show_error(const char *message) {
    fl_alert("%s", message);
}

const char *ask_value(const char *message) {
    return fl_input (message, "");
}

void new_video_file_chooser(Fl_Native_File_Chooser *dialog, const char *filter, const char *title, bool saving=true) {
    if (saving) {
        dialog->options(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    }
    dialog->title(title);
    dialog->filter(filter);
}

std::string execute_file_choose(Fl_Native_File_Chooser *fc, std::string* current_dir, const char *default_extension) {
    std::string path;

    if (current_dir && !current_dir->empty()) {
        if (filepath_exists(current_dir->c_str())) {
            fc->directory(current_dir->c_str());
        }
    }

    if (fc->show() == 0) {
        path = fc->filename();

        if (current_dir) {
            auto dir = boost::filesystem::path(path).parent_path().string();
            if (filepath_exists(dir.c_str())) {
                *current_dir = boost::filesystem::path(path).parent_path().string();
            }
        }

        int extension_lenght = default_extension ? strlen(default_extension) - 1 : 3;

        if (default_extension && (path.size() <= extension_lenght || *(path.rbegin() + extension_lenght) != '.')) {
            path += default_extension;
        }
    }

    return path;
}

std::string input_video_file_chooser(std::string* current_dir, const char *default_extension) {
    Fl_Native_File_Chooser dialog(Fl_Native_File_Chooser::BROWSE_FILE);
    new_video_file_chooser(&dialog, kINPUT_VIDEO_FILE_FILTER, kINPUT_VIDEO_FILE_TITLE, false);
    return execute_file_choose(&dialog, current_dir, default_extension);
}

std::string output_video_file_chooser(std::string* current_dir, const char *default_extension) {
    Fl_Native_File_Chooser dialog(Fl_Native_File_Chooser::BROWSE_FILE);
    new_video_file_chooser(&dialog, kOUTPUT_VIDEO_FILE_FILTER, kOUTPUT_VIDEO_FILE_TITLE);
    return execute_file_choose(&dialog, current_dir, default_extension);
}

std::string output_mjpeg_file_chooser(std::string* current_dir, const char *default_extension) {
    Fl_Native_File_Chooser dialog(Fl_Native_File_Chooser::BROWSE_FILE);
    new_video_file_chooser(&dialog, kOUTPUT_MJPEG_FILE_FILTER, kOUTPUT_VIDEO_FILE_TITLE);
    return execute_file_choose(&dialog, current_dir, default_extension);
}

std::string output_webm_file_chooser(std::string* current_dir, const char *default_extension) {
    Fl_Native_File_Chooser dialog(Fl_Native_File_Chooser::BROWSE_FILE);
    new_video_file_chooser(&dialog, kOUTPUT_WEBM_FILE_FILTER, kOUTPUT_VIDEO_FILE_TITLE);
    return execute_file_choose(&dialog, current_dir, default_extension);
}

std::string input_prj_file_chooser(std::string* current_dir, const char *default_extension) {
    Fl_Native_File_Chooser dialog(Fl_Native_File_Chooser::BROWSE_FILE);
    new_video_file_chooser(&dialog, kINPUT_PROJECT_FILE_FILTER, kINPUT_PROJECT_FILE_TITLE, false);
    return execute_file_choose(&dialog, current_dir, default_extension);
}

std::string output_prj_file_chooser(std::string* current_dir, const char *default_extension) {
    Fl_Native_File_Chooser dialog(Fl_Native_File_Chooser::BROWSE_FILE);
    new_video_file_chooser(&dialog, kOUTPUT_PROJECT_FILE_FILTER, kOUTPUT_PROJECT_FILE_TITLE);
    return execute_file_choose(&dialog, current_dir, default_extension);

}



}  // namespace vcutter

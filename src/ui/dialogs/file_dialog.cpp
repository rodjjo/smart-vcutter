/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <fstream>

#include <jsoncpp/json/json.h>
#include <boost/filesystem.hpp>
#include <FL/Fl_Native_File_Chooser.H>

#include "src/common/utils.h"
#include "src/ui/dialogs/file_dialog.h"


namespace vcutter {

namespace {

Json::Value load_json_file(const std::string & path) {
    std::ifstream ifile(path.c_str(), std::ios_base::in);
    Json::Value root;
    Json::Reader reader;

    if (reader.parse(ifile, root, false)) {
        return root;
    }

    return Json::Value();
}

std::string get_history_filepath() {
    return temp_filepath("smart-vcutter-paths.json");
}

}  // namespace


FileDialog::FileDialog(filedialog_type dialog_type, bool opening) {
    dialog_type_ = dialog_type;
    opening_ = opening;
}

FileDialog::~FileDialog() {
}

void FileDialog::load_history() {
    Json::Value root = load_json_file(get_history_filepath());
    if (root.isMember(history_key())) {
        history_ = root[history_key()].asString();
    }
}

const char *FileDialog::history_key() {
    if (opening_) {
        switch (dialog_type_) {
            case filedialog_project:
                return "last_opened_prj_dir";
            case filedialog_project_or_video:
                return "last_opened_prj_or_video_dir";
            case  filedialog_webm:
                return "last_opened_webm_dir";
            case filedialog_mp4:
                return "last_opened_mp4_dir";
            case filedialog_mjpeg:
                return  "last_opened_mjpeg_dir";
            default:;
        }
        return "last_opened_video_dir";
    }

    switch (dialog_type_) {
        case filedialog_project:
            return "last_saved_prj_dir";
        case filedialog_project_or_video:
            return "last_saved_prj_or_video_dir";
        case  filedialog_webm:
            return "last_saved_webm_dir";
        case filedialog_mp4:
            return "last_saved_mp4_dir";
        case filedialog_mjpeg:
            return  "last_saved_mjpeg_dir";
        default:;
    }

    return "last_saved_video_dir";
}

const char *FileDialog::filter() {
    switch (dialog_type_) {
        case filedialog_project:
            return "Clipping project\t*.vcutter\n";
        case filedialog_project_or_video:
            return "Supported files\t*.{vcutter,avi,mp4,mkv,mpeg,wmv,mov}\n";
        case  filedialog_webm:
            return "WEBM Videos\t*.webm\n";
        case filedialog_mp4:
            return "MP4 Videos\t*.mp4\n";
        case filedialog_mjpeg:
            return  "MJPEG Videos\t*.mp4\n";
        default:;
    }

    if (opening_) {
        return "Video files\t*.{avi,mp4,mkv,mpeg,wmv,mov}\n";
    }

    return "Video files\t*.{mp4,webm,mjpeg}\n";
}

const char *FileDialog::title() {
    if (opening_) {
        switch (dialog_type_) {
            case filedialog_project:
                return "Open clipping project";
            case filedialog_project_or_video:
                return "Open file";
            case  filedialog_webm:
                return "Open webm video";
            case filedialog_mp4:
                return "Open mp4 video";
            case filedialog_mjpeg:
                return "Open mjpeg video";
            default:;
        }
        return "Open video file";
    }

    switch (dialog_type_) {
        case filedialog_project:
            return "Save clipping project";
        case filedialog_project_or_video:
            return "Save file";
        case  filedialog_webm:
            return "Save webm video";
        case filedialog_mp4:
            return "Save mp4 video";
        case filedialog_mjpeg:
            return "Save mjpeg video";
        default:;
    }

    return "Save video file";
}

void FileDialog::save_history() {
    Json::Value root = load_json_file(get_history_filepath());
    root[history_key()] = history_;
    std::ofstream ofile(get_history_filepath(), std::ios_base::trunc);
    ofile << Json::FastWriter().write(root);
}

std::string FileDialog::choose_path() {
    load_history();

    Fl_Native_File_Chooser dialog(Fl_Native_File_Chooser::BROWSE_FILE);

    if (!opening_) {
        dialog.options(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    }

    dialog.title(title());
    dialog.filter(filter());

    if (filepath_exists(history_.c_str())) {
        dialog.directory(history_.c_str());
    }

    if (dialog.show() != 0) {
        return std::string();
    }

    std::string path = dialog.filename();

    if (!path.empty()) {
        auto dir = boost::filesystem::path(path).parent_path().string();
        if (filepath_exists(dir.c_str())) {
            history_ = dir;
        }
        save_history();
    }

    return path;
}

}  // namespace vcutter

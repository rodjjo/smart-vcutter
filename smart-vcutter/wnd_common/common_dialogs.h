/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_WND_COMMON_COMMON_DIALOGS_H_
#define SMART_VCUTTER_WND_COMMON_COMMON_DIALOGS_H_

#include <string>
#include <FL/Fl_Native_File_Chooser.H>

namespace vcutter {

std::string choose_video_file();

bool ask(const char *message);
int yes_nc(const char *message);

void show_error(const char *message);
const char *ask_value(const char *message);

std::string input_video_file_chooser(std::string* current_dir=NULL, const char *default_extension = NULL);
std::string output_video_file_chooser(std::string* current_dir=NULL, const char *default_extension = ".mp4");
std::string output_mjpeg_file_chooser(std::string* current_dir=NULL, const char *default_extension = ".mp4");
std::string output_webm_file_chooser(std::string* current_dir=NULL, const char *default_extension = ".webm");
std::string input_prj_file_chooser(std::string* current_dir=NULL, const char *default_extension = NULL);
std::string output_prj_file_chooser(std::string* current_dir=NULL, const char *default_extension = ".vcutter");

}  // namespace vcutter

#endif  // SMART_VCUTTER_WND_COMMON_COMMON_DIALOGS_H_

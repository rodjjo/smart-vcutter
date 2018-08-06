/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_COMMON_UTILS_H_
#define SMART_VCUTTER_COMMON_UTILS_H_

#include <string>

namespace vcutter {

bool filepath_exists(const char *path);
std::string generate_path(const std::string& prefix, const std::string& sufix, int index);
std::string change_filepath_dir(const std::string& source_path, const std::string& directory, const char* new_extension, bool checkseq=true);
bool is_same_path(const char *path1, const char *path2);
std::string temp_filepath(const char *filename);
bool remove_file(const char *filename);

}  // namespace vcutter


#endif  // SMART_VCUTTER_COMMON_UTILS_H_
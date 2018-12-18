/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cstdio>
#include <regex> // NOLINT
#include <boost/filesystem.hpp>
#include "src/common/utils.h"

namespace vcutter {

std::string change_filepath_dir(const char *source_path, const char *directory, const char *new_extension, bool checkseq) {
    auto src_path = boost::filesystem::path(source_path);
    auto name = src_path.stem();
    auto output_dir = boost::filesystem::exists(directory) ? directory : src_path.parent_path().string();

    if (!checkseq) {
        auto result = boost::filesystem::path(output_dir) / (name.string() + new_extension);
        if (!boost::filesystem::exists(result)) {
            return result.string();
        }
    }

    char tmp[25];
    int index = 1;
    do {
        snprintf(tmp, sizeof(tmp), ".%03d", index++);
        auto result = boost::filesystem::path(output_dir) / (name.string() + tmp + new_extension);
        if (!boost::filesystem::exists(result)) {
            return result.string();
        }
    } while(true);

    return std::string();
}

bool filepath_exists(const char *path) {
    return boost::filesystem::exists(path);
}

std::string generate_path(const char *prefix, const char *sufix, int start_number) {
    std::string result;

    char tmp[25];
    do {
        snprintf(tmp, sizeof(tmp), "%03d", start_number++);
        result = std::string(prefix) + tmp + sufix;
    } while(boost::filesystem::exists(result));

    return result;
}

bool remove_file(const char *filename) {
    return std::remove(filename) == 0 || !filepath_exists(filename);
}

int str_to_milliseconds(const char *timestr) {
    unsigned int hours, minutes, seconds, ms;
    int scan = sscanf(timestr, "%u:%u:%u,%u", &hours, &minutes, &seconds, &ms);

    if (scan != 4) {
        ms = 0;
        scan = sscanf(timestr, "%u:%u:%u", &hours, &minutes, &seconds);
    }

    if (scan >= 3) {
        return (hours * 3600 + minutes * 60 + seconds) * 1000 + ms;
    }

    scan = sscanf(timestr, "%u:%u", &minutes, &seconds);

    if (scan == 2) {
        return (minutes * 60 + seconds) * 1000;
    }

    return 0;
}

std::string temp_filepath(const char *filename) {
    if (filename) {
        return (boost::filesystem::temp_directory_path() / boost::filesystem::path(filename)).string();
    }

    return boost::filesystem::temp_directory_path().string();
}

void seconds_to_str(char *buffer, int size, double tmr_in_seconds, bool ms) {
    int seconds = static_cast<int>(tmr_in_seconds * 1000);
    int hours = (seconds / (1000 * 60 * 60));
    int mins = (seconds / (1000 * 60)) % 60;
    seconds = (seconds / 1000) % 60;
    int milli = (tmr_in_seconds - static_cast<int>(tmr_in_seconds)) * 1000;

    if (ms) {
        snprintf(buffer, size, "%02d:%02d:%02d,%03d", hours, mins, seconds, milli);
    } else {
        snprintf(buffer, size, "%02d:%02d:%02d", hours, mins, seconds);
    }
}

bool is_video_path(const std::string& path) {
    std::regex project_ext_regex("^.*\\.vcutter$", std::regex_constants::icase);
    return std::regex_match(path, project_ext_regex);
}

}  // namespace vcutter

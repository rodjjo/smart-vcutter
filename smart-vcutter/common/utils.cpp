#include <cstdio>
#include <boost/filesystem.hpp>
#include "smart-vcutter/common/utils.h"

namespace vcutter {

bool filepath_exists(const char *path) {
    return boost::filesystem::exists(path);
}

bool is_same_path(const char *path1, const char *path2) {
    return boost::filesystem::path(path1) == boost::filesystem::path(path2);
}

std::string temp_filepath(const char *filename) {
    if (filename) {
        return (boost::filesystem::temp_directory_path() / boost::filesystem::path(filename)).string();
    }

    return boost::filesystem::temp_directory_path().string();
}

bool remove_file(const char *filename) {
    return std::remove(filename) == 0;
}

std::string generate_path(const std::string& prefix, const std::string& sufix, int index) {
    std::string result;
    char tmp[25];
    do {
        snprintf(tmp, sizeof(tmp), "%03d", index++);
        result = prefix + tmp + sufix;
    } while(boost::filesystem::exists(result));
    return result;
}

std::string change_filepath_dir(const std::string& source_path, const std::string& directory, const char* new_extension, bool checkseq) {
    auto p = boost::filesystem::path(source_path);
    auto stem = p.stem();
    auto output_dir = boost::filesystem::exists(directory) ? directory : p.parent_path().string();

    if (!checkseq) {
        auto result = boost::filesystem::path(output_dir) / (stem.string() + new_extension);
        if (!boost::filesystem::exists(result)) {
            return result.string();
        }
    }

    char tmp[25];
    int index = 1;
    do {
        snprintf(tmp, sizeof(tmp), ".%03d", index++);
        auto result = boost::filesystem::path(output_dir) / (stem.string() + tmp + new_extension);
        if (!boost::filesystem::exists(result)) {
            return result.string();
        }
    } while(true);

    return std::string();
}

}
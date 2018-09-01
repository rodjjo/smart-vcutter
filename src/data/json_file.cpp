/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
*/
#include <fstream>
#include "src/common/utils.h"
#include "src/data/json_file.h"

namespace vcutter {

JsonFile::JsonFile(
    const char *path, bool delete_on_close, bool should_load
) {
    path_ = path;
    delete_on_close_ = delete_on_close;
    loaded_ = false;
    if (should_load) {
        load();
    }
}

void JsonFile::load() {
    loaded_ = false;
    if (filepath_exists(path_.c_str())) {
        std::ifstream ifile;
        ifile.open(path_.c_str());
        if (ifile.is_open()) {
            Json::Reader reader;
            Json::Value temp;

            if (!reader.parse(ifile, temp, false)) {
                return;
            }

            root_ = temp;
            loaded_ = true;
        }
    }
}

JsonFile::~JsonFile() {
    if (loaded_ && delete_on_close_) {
        remove_file(path_.c_str());
    }
}

bool JsonFile::loaded() {
    return loaded_;
}

bool JsonFile::save(const Json::Value& data) {
    root_ = data;
    return save();
}

bool JsonFile::save() {
    std::ofstream ofile;
    ofile.open(path_.c_str());

    if (ofile.is_open()) {
        Json::FastWriter writer;
        ofile << writer.write(root_);
        loaded_ = true;
        return true;
    }

    return false;
}

Json::Value & JsonFile::operator[] (const char *key) {
    return root_[key];
}

const Json::Value & JsonFile::get_data() {
    return root_;
}

const Json::Value & JsonFile::operator[] (const char *key) const {
    return root_[key];
}

}  // namespace vcutter

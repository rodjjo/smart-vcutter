/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <fstream>
#include "smart-vcutter/common/utils.h"
#include "smart-vcutter/data/session.h"

namespace vcutter {

namespace {
const char *kSESSION_FILENAME = "smart-vcutter-session-";
const char *kSESSION_EXTENSION = ".json";
}

Session::Session(const char *sufix, bool auto_remove_file) {
    auto_remove_file_ = auto_remove_file;
    path_ = temp_filepath(kSESSION_FILENAME);
    path_ += sufix;
    path_ += kSESSION_EXTENSION;
    loaded_ = false;
    saved_ = false;
    last_saved_version_ = static_cast<uint64_t>(-1);
    load();
}

Session::~Session() {
    if (auto_remove_file_) {
        remove_file(path_.c_str());
    }
}

void Session::load() {
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

void Session::save() {
    saved_ = true; // do not retry saving session
    std::ofstream ofile;
    ofile.open(path_.c_str());

    if (ofile.is_open()) {
        Json::FastWriter writer;
        ofile << writer.write(root_);
    }
}

void Session::set_value(const char *key, const char* value) {
    root_[key] = value;
}

bool Session::loaded() {
    return loaded_;
}

bool Session::saved() {
    return saved_;
}

uint64_t Session::get_version() {
    return last_saved_version_;
}

void Session::save(uint64_t version, const Json::Value& data) {
    root_ = data;
    last_saved_version_ = version;
    save();
}

void Session::save(uint64_t version, const session_data_t& data) {
    last_saved_version_ = version;
    root_ = Json::Value();
    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        root_[it->first] = it->second;
    }
    save();
}

session_data_t Session::get_value_map() {
    session_data_t result;
    for (const auto & name : root_.getMemberNames()) {
        if (root_[name].isString()) {
            result[name] = root_[name].asString();
        }
    }
    return result;
}

const Json::Value & Session::get_data() {
    return root_;
}

}  // namespace vcutter

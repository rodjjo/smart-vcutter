/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <sstream>
#include "src/common/utils.h"
#include "src/data/history.h"

namespace vcutter {

History::History(const char *path) {
    std::string histpath = path ? path : history_file_path();
    json_file_.reset(new JsonFile(histpath.c_str()));
}

History::~History() {
}

std::string History::operator[] (const char *key) {
    return (*json_file_)[key].asString();
}

void History::set(const char *key, const char *value) {
    (*json_file_)[key] = value;
    json_file_->save();
}

std::string History::history_file_path() {
    return temp_filepath("smart-vcutter-history.json");
}

}   // namespace vcutter

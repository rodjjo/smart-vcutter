/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <fstream>
#include <boost/filesystem.hpp>
#include "smart-vcutter/common/utils.h"
#include "smart-vcutter/data/project.h"

namespace vcutter {

namespace {
    const char *kPROJECT_CLIPPING_KEY = "clipping";
    const char *kPROJECT_CLIPPING_PATH_KEY = "path";
    const char *kPROJECT_CLIPPING_WIDTH_KEY = "width";
    const char *kPROJECT_CLIPPING_HEIGHT_KEY = "height";
    const char *kPROJECT_CLIPPING_ITEMS_KEY = "items";
    const char *kPROJECT_CLIPPING_FRAME_KEY = "frame";
    const char *kPROJECT_CLIPPING_PX_KEY = "px";
    const char *kPROJECT_CLIPPING_PY_KEY = "py";
    const char *kPROJECT_CLIPPING_RX1_KEY = "rx1";
    const char *kPROJECT_CLIPPING_RX2_KEY = "rx2";
    const char *kPROJECT_CLIPPING_RY1_KEY = "ry1";
    const char *kPROJECT_CLIPPING_RY2_KEY = "ry2";
    const char *kPROJECT_CLIPPING_RFRAME_KEY = "ref_frame";
    const char *kPROJECT_CLIPPING_REF_KEY = "has_ref";
    const char *kPROJECT_CLIPPING_SCALE_KEY = "scale";
    const char *kPROJECT_CLIPPING_ANGLE_KEY = "rotation";
}  // namespace

Project::Project() {
}

Project::Project(const std::string& path) {
    open(path);
}

bool Project::open(const Json::Value& data, const std::string& path) {
    if (!data.isMember(kPROJECT_CLIPPING_KEY) || data.isMember(kPROJECT_CLIPPING_PATH_KEY)) {
        last_error_ = "This is not a valid project. Missing clipping information or video path";
        return false;
    }
    path_ = path;
    project_ = data;
    return true;
}

bool Project::load(const std::string& data, const char *path) {
    Json::Reader reader;
    Json::Value temp;

    if (!reader.parse(data, temp, false)) {
        last_error_ = "The project file is corrupted or invalid";
        return false;
    }

    return open(temp, path ? path : "");
}

bool Project::open(const std::string& path) {
    if (filepath_exists(path.c_str())) {
        std::ifstream ifile;
        ifile.open(path.c_str());
        if (ifile.is_open()) {
            Json::Reader reader;
            Json::Value temp;

            if (!reader.parse(ifile, temp, false)) {
                last_error_ = "The project file is corrupted or invalid";
                return false;
            }

            return open(temp, path);
        }
        last_error_ = "could not open the project file";
        return false;
    }

    last_error_ = "this directory does not contain a project";
    return false;
}

Project::~Project() {
}

std::string Project::get_last_error() {
    return last_error_;
}

bool Project::save() {
    if (save_json(project_, path_)) {
        return true;
    }

    last_error_ = "could not save the project file";
    return false;
}

bool Project::save(const std::string& path) {
    if (save_json(project_, path)) {
        return true;
    }

    last_error_ = "could not save the project file";
    return false;
}

bool Project::save_json(const Json::Value& root, const std::string& path, bool change_path) {
    std::ofstream ofile;
    ofile.open(path.c_str());

    if (ofile.is_open()) {
        Json::FastWriter writer;
        ofile << writer.write(root);
        if (change_path) {
            path_ = path;
        }
        return true;
    }

    if (change_path) {
        path_.clear();
    }
    return false;
}

bool Project::is_open() {
    return !path_.empty();
}

std::string Project::get_path() {
    return path_;
}


const clipping_t *Project::get_clipping() {
    if (!project_.isMember(kPROJECT_CLIPPING_KEY)) {
        return NULL;
    }

    clipping_ = clipping_t();

    Json::Value & data = project_[kPROJECT_CLIPPING_KEY];

    clipping_.video_path = data[kPROJECT_CLIPPING_PATH_KEY].asString();
    clipping_.w = data[kPROJECT_CLIPPING_WIDTH_KEY].asDouble();
    clipping_.h = data[kPROJECT_CLIPPING_HEIGHT_KEY].asDouble();

    clipping_.rx1 = data[kPROJECT_CLIPPING_RX1_KEY].asDouble();
    clipping_.rx2 = data[kPROJECT_CLIPPING_RX2_KEY].asDouble();
    clipping_.ry1 = data[kPROJECT_CLIPPING_RY1_KEY].asDouble();
    clipping_.ry2 = data[kPROJECT_CLIPPING_RY2_KEY].asDouble();

    clipping_.ref_frame = data[kPROJECT_CLIPPING_RFRAME_KEY].asInt();
    clipping_.has_ref = data[kPROJECT_CLIPPING_REF_KEY].asBool();

    Json::Value & items = data[kPROJECT_CLIPPING_ITEMS_KEY];

    for (Json::Value::ArrayIndex i = 0; i < items.size(); ++i) {
        Json::Value & item = items[i];
        clipping_key_t key;
        key.frame = item[kPROJECT_CLIPPING_FRAME_KEY].asInt();
        key.px = item[kPROJECT_CLIPPING_PX_KEY].asDouble();
        key.py = item[kPROJECT_CLIPPING_PY_KEY].asDouble();
        key.scale = item[kPROJECT_CLIPPING_SCALE_KEY].asDouble();
        key.angle(item[kPROJECT_CLIPPING_ANGLE_KEY].asDouble());
        clipping_.items.push_back(key);
    }

    return &clipping_;
}

Json::Value Project::data_from_clipping(const clipping_t& clipping) {
    Json::Value data;
    data[kPROJECT_CLIPPING_PATH_KEY] = clipping.video_path;
    data[kPROJECT_CLIPPING_WIDTH_KEY] = clipping.w;
    data[kPROJECT_CLIPPING_HEIGHT_KEY] = clipping.h;

    data[kPROJECT_CLIPPING_RX1_KEY] = clipping.rx1;
    data[kPROJECT_CLIPPING_RX2_KEY] = clipping.rx2;
    data[kPROJECT_CLIPPING_RY1_KEY] = clipping.ry1;
    data[kPROJECT_CLIPPING_RY2_KEY] = clipping.ry2;
    data[kPROJECT_CLIPPING_RFRAME_KEY] = clipping.ref_frame;
    data[kPROJECT_CLIPPING_REF_KEY] = clipping.has_ref;

    for (const auto &item : clipping.items) {
        Json::Value element;
        element[kPROJECT_CLIPPING_FRAME_KEY] = item.frame;
        element[kPROJECT_CLIPPING_PX_KEY] = item.px;
        element[kPROJECT_CLIPPING_PY_KEY] = item.py;
        element[kPROJECT_CLIPPING_SCALE_KEY] = item.scale;
        element[kPROJECT_CLIPPING_ANGLE_KEY] = item.angle();
        data[kPROJECT_CLIPPING_ITEMS_KEY].append(element);
    }
    return data;
}

void Project::set_clipping(const clipping_t& clipping) {
    project_[kPROJECT_CLIPPING_KEY] = data_from_clipping(clipping);
}

const Json::Value & Project::get_data() {
    return project_;
}


}   // namespace vcutter

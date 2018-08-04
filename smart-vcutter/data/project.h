#ifndef SMART_VCUTTER_DATA_VCUTTER_PROJECT_H
#define SMART_VCUTTER_DATA_VCUTTER_PROJECT_H

#include <string.h>
#include <memory>
#include <string>
#include <list>
#include <jsoncpp/json/json.h>

namespace vcutter {

typedef struct clipping_key {
    clipping_key() {
        frame = 0;
        px = 0;
        py = 0;
        scale = 0;
        angle = 0;
    }

    unsigned int frame;
    double px;
    double py;
    double scale;
    double angle;
} clipping_key_t;

typedef std::list<clipping_key_t> clipping_key_list_t;

typedef struct clipping {
    clipping() {
        w = 0;
        h = 0;
        has_ref = false;
        rx1 = 0;
        ry1 = 0;
        rx2 = 0;
        ry2 = 0;
        ref_frame = 0;
    }

    std::string video_path;

    float w;
    float h;

    double rx1;
    double ry1;
    double rx2;
    double ry2;
    unsigned int ref_frame;
    bool has_ref;

    clipping_key_list_t items;
} clipping_t;

class Project {
 public: 
    Project(const std::string& path);
    Project();
    ~Project();
    bool open(const std::string& path);
    bool open(const Json::Value& data, const std::string& path);
    bool load(const std::string& data, const char *path = NULL);
    bool is_open();
    bool save();
    bool save(const std::string& path);
    std::string get_path();
    std::string get_last_error();
    const clipping_t *get_clipping();
    void set_clipping(const clipping_t& clipping);
    const Json::Value & get_data();
 private:
    bool save_json(const Json::Value& root, const std::string& path, bool change_path = true);
    Json::Value data_from_clipping(const clipping_t& clipping);
 private:
    clipping_t clipping_;
    std::string last_error_;
    std::string path_;
    Json::Value project_;
};


}

#endif  // SMART_VCUTTER_DATA_VCUTTER_PROJECT_H

/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_DATA_JSON_FILE_H_
#define SRC_DATA_JSON_FILE_H_

#include <string>
#include <jsoncpp/json/json.h>

namespace vcutter {

class JsonFile {
 public:
    JsonFile(const char *path, bool delete_on_close=false, bool should_load=true);
    virtual ~JsonFile();
    bool loaded();
    bool save(const Json::Value& data);
    bool save();
    Json::Value & operator[] (const char *key);
    const Json::Value & operator[] (const char *key) const;
    const Json::Value & get_data();
 private:
    void load();

 private:
    bool loaded_;
    Json::Value root_;
    std::string path_;
    bool delete_on_close_;
};

}  // namespace vcutter

#endif  // SRC_DATA_JSON_FILE_H_

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
    JsonFile(const char *path, bool delete_on_close=false);
    virtual ~JsonFile();
    virtual bool loaded();
    virtual bool save(const Json::Value& data);
    virtual const Json::Value & get_data();
 private:
    bool loaded_;
    Json::Value root_;
    bool delete_on_close_;
};

}  // namespace vcutter

#endif  // SRC_DATA_JSON_FILE_H_

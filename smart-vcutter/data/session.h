/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_DATA_SESSION_H_
#define SMART_VCUTTER_DATA_SESSION_H_

#include <inttypes.h>
#include <string>
#include <map>
#include <jsoncpp/json/json.h>


namespace vcutter {

typedef std::map<std::string, std::string> session_data_t;

class Session {
 public:
    Session(const char *sufix, bool auto_remove_file=true);
    virtual ~Session();
    bool loaded();
    uint64_t get_version();
    void save(uint64_t version, const Json::Value& data);
    void save(uint64_t version, const session_data_t& data);
    session_data_t get_value_map();
    const Json::Value & get_data();
    bool saved();

 protected:
    void load();
    void save();
    void set_value(const char *key, const char* value);

 private:
    bool saved_;
    bool loaded_;
    bool auto_remove_file_;
    uint64_t last_saved_version_;
    Json::Value root_;
    std::string path_;
};

}  // namespace vcutter

#endif  // SMART_VCUTTER_DATA_SESSION_H_

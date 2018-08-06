/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_DATA_HISTORY_H_
#define SMART_VCUTTER_DATA_HISTORY_H_

#include <string>
#include "smart-vcutter/data/session.h"

namespace vcutter {

class History : private Session {
  public:
    History();
    virtual ~History();
    std::string get(const char *key);
    void set(const char *key, const char* value);
};

}  // namespace vcutter

#endif // SMART_VCUTTER_DATA_HISTORY_H_

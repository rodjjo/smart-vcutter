#ifndef SMART_VCUTTER_DATA_HISTORY_H
#define SMART_VCUTTER_DATA_HISTORY_H

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

#endif // SMART_VCUTTER_DATA_HISTORY_H
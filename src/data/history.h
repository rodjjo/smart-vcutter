/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_DATA_HISTORY_H_
#define SRC_DATA_HISTORY_H_

#include <string>
#include "src/data/session.h"

namespace vcutter {

class History : private Session {
  public:
    History();
    virtual ~History();
    std::string get(const char *key);
    void set(const char *key, const char* value);
};

}  // namespace vcutter

#endif // SRC_DATA_HISTORY_H_

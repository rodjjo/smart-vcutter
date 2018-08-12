/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_DATA_HISTORY_H_
#define SRC_DATA_HISTORY_H_

#include <memory>
#include <string>

#include "src/data/json_file.h"

namespace vcutter {

class History {
  public:
    History(const char *path=NULL);
    virtual ~History();
    std::string operator[] (const char *key);
    void set(const char *key, const char *value);

    static std::string history_file_path();

  private:
    std::unique_ptr<JsonFile> json_file_;
};

}  // namespace vcutter

#endif // SRC_DATA_HISTORY_H_

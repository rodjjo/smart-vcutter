/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include "src/data/history.h"

namespace vcutter {

namespace {
    const char *kHISTORY_PREFIX = "history";
}

History::History(): Session(kHISTORY_PREFIX, false) {
}

History::~History() {
}

std::string History::get(const char *key) {
    auto d = get_data();
    if (d.isMember(key)) {
        return d[key].asString();
    }
    return std::string();
}

void History::set(const char *key, const char* value) {
    set_value(key, value);
    save();
}

}   // namespace vcutter

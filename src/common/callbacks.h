/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SRC_COMMON_CALLBACKS_H_
#define SRC_COMMON_CALLBACKS_H_

#include <functional>

typedef std::function<void()> callback_t;
typedef std::function<bool()> bool_callback_t;

#endif // SRC_COMMON_CALLBACKS_H_

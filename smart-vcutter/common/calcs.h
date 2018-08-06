/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_COMMON_CALCS_H_
#define SMART_VCUTTER_COMMON_CALCS_H_

#include <inttypes.h>

#include "smart-vcutter/common/point.h"
#include "smart-vcutter/common/box.h"
#include "smart-vcutter/common/view_port.h"

namespace vcutter {

// Ensure the angle is interval 0..360
float normalize_angle(float angle);

}  // namespace vcutter

#endif  // SMART_VCUTTER_COMMON_CALCS_H_

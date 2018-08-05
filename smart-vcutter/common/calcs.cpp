#include <cmath>
#include <string.h>
#include "smart-vcutter/common/calcs.h"

namespace vcutter {

float normalize_angle(float angle) {
    while (angle < 0) angle += 360;
    while (angle > 360) angle -= 360;
    return angle;
}

}  // namespace vcutter

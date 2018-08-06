/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#include <cstdio>
#include "smart-vcutter/common/conversions.h"


namespace vcutter {

void time_to_str(char *buffer, int size, double time, bool ms) {
    int seconds = static_cast<int>(time * 1000);
    int hours = (seconds / (1000 * 60 * 60));
    int mins = (seconds / (1000 * 60)) % 60;
    seconds = (seconds / 1000) % 60;
    int milli = (time - static_cast<int>(time)) * 1000;

    if (ms) {
        snprintf(buffer, size, "%02d:%02d:%02d,%03d", hours, mins, seconds, milli);
    } else {
        snprintf(buffer, size, "%02d:%02d:%02d", hours, mins, seconds);
    }
}

int str_to_time(const char *timestr) {
    unsigned int hours, minutes, seconds, ms;
    int scan = sscanf(timestr, "%u:%u:%u,%u", &hours, &minutes, &seconds, &ms);

    if (scan != 4) {
        ms = 0;
        scan = sscanf(timestr, "%u:%u:%u", &hours, &minutes, &seconds);
    }

    if (scan >= 3) {
        return (hours * 36000 + minutes * 60 + seconds) * 1000 + ms;
    }

    scan = sscanf(timestr, "%u:%u", &minutes, &seconds);

    if (scan >= 2) {
        return (minutes * 60 + seconds) * 1000;
    }

    return 0;
}

}
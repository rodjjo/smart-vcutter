/*
 * Copyright (C) 2018 by Rodrigo Antonio de Araujo
 */
#ifndef SMART_VCUTTER_COMMON_CONVERSIONS_H_
#define SMART_VCUTTER_COMMON_CONVERSIONS_H_

namespace vcutter {

void time_to_str(char *buffer, int size, double time, bool ms=false);
int str_to_time(const char *timestr);

}  // namespace vcutter

#endif  // SMART_VCUTTER_COMMON_CONVERSIONS_H_
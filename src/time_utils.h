#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <time.h>

extern const char* NTP_SERVER;
extern const long UTC_OFFSET_SECONDS;

void init_time_config();
String get_current_time();

#endif
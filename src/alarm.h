#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>
#include "mqtt.h"
#include "storage.h"
#include "time_utils.h"
#define BEEP_PIN 14
void init_alarm();
void start_alarm();
void stop_alarm();
void open_defence();
void close_defence();
#endif
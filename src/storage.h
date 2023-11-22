#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#define EEPROM_DEFENCE_STATE_OFFSET 0
#define EEPROM_STOP_TIME_OFFSET 32
#define EEPROM_START_TIME_OFFSET 64

extern bool _defence_state;
extern bool _alarm_state;
extern String _start_defence_time;
extern String _stop_defence_time;

void load_config();
void write_start_defence_time(String time);
void write_stop_defence_time(String time);
void write_defence_state(bool defence_state);
#endif
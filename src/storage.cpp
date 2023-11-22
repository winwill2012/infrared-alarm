#include "storage.h"

bool _defence_state;
bool _alarm_state = false;
String _start_defence_time;
String _stop_defence_time;

void load_config() {
    EEPROM.begin(128);
    _defence_state = EEPROM.readBool(EEPROM_DEFENCE_STATE_OFFSET);
    _start_defence_time = EEPROM.readString(EEPROM_START_TIME_OFFSET);
    _stop_defence_time = EEPROM.readString(EEPROM_STOP_TIME_OFFSET);
    Serial.println("=========Load config=========");
    Serial.print("defence_state:      ");
    Serial.println(_defence_state);
    Serial.println("start_defence_time: " + _start_defence_time);
    Serial.println("stop_defence_time:  " + _stop_defence_time);
}

void write_start_defence_time(String time) {
    _start_defence_time = time;
    EEPROM.writeString(EEPROM_START_TIME_OFFSET, time);
    EEPROM.commit();
}

void write_stop_defence_time(String time) {
    _stop_defence_time = time;
    EEPROM.writeString(EEPROM_STOP_TIME_OFFSET, time);
    EEPROM.commit();
}

void write_defence_state(bool defence_state) {
    _defence_state = defence_state;
    EEPROM.writeBool(EEPROM_DEFENCE_STATE_OFFSET, defence_state);
    EEPROM.commit();
}
#include "alarm.h"

void init_alarm() {
    pinMode(BEEP_PIN, OUTPUT);
    digitalWrite(BEEP_PIN, LOW);
}

bool validate_time() {
    String curr_time = get_current_time();
    if (_start_defence_time.compareTo(_stop_defence_time) <= 0) {
        return curr_time.compareTo(_start_defence_time) >= 0 &&
               curr_time.compareTo(_stop_defence_time) <= 0;
    } else {
        return (curr_time.compareTo(_start_defence_time) >= 0 &&
                curr_time.compareTo("23:59:59") <= 0) ||
               (curr_time.compareTo("00:00:00") >= 0 &&
                curr_time.compareTo(_stop_defence_time) <= 0);
    }
}

void start_alarm() {
    String curr_time = get_current_time();
    if (_defence_state && !_alarm_state && validate_time()) {
        _alarm_state = true;
        digitalWrite(BEEP_PIN, HIGH);
        publish_message("{\"alarm_state\": true}");
    }
}

void stop_alarm() {
    digitalWrite(BEEP_PIN, LOW);
    publish_message("{\"alarm_state\": false, \"defence_state\": false}");
    _alarm_state = false;
}

void open_defence() {
    for (int i = 0; i < 2; i++) {
        digitalWrite(BEEP_PIN, HIGH);
        vTaskDelay(300);
        digitalWrite(BEEP_PIN, LOW);
        vTaskDelay(300);
    }
}

void close_defence() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(BEEP_PIN, HIGH);
        vTaskDelay(200);
        digitalWrite(BEEP_PIN, LOW);
        vTaskDelay(200);
    }
}
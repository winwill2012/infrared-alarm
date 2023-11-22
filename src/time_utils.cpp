#include "time_utils.h"

const char* NTP_SERVER = "ntp.aliyun.com";
const long UTC_OFFSET_SECONDS = 28800;

void init_time_config() {
    configTime(UTC_OFFSET_SECONDS, 0, NTP_SERVER);
    while (time(nullptr) == 0) {
        vTaskDelay(1000);
        Serial.println("Waiting for time sync...");
    }
    Serial.println("Time synced successfully");
}

String get_current_time() {
    time_t now = time(nullptr);
    String current_time = ctime(&now);
    int index = current_time.indexOf(":");
    String t = current_time.substring(index - 2, index + 6);
    Serial.println("current_time: " + t);
    return t;
}

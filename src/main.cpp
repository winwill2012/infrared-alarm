#include <Arduino.h>
#include <WiFi.h>
#include "WiFiManager.h"
#include "alarm.h"
#include "mqtt.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"
#include "soc/soc.h"
#include "storage.h"
#include "time_utils.h"

const String TELECONTROL_ON[2] = {"LC:BD8D347E", "LC:ADF49435"};
const String TELECONTROL_OFF[2] = {"LC:BD8D327C", "LC:ADF49233"};

// 查询指令类型，0：关闭，1：打开，2：传感器信号
int get_cmd_type(String cmd) {
    for (String on : TELECONTROL_ON) {
        if (cmd.indexOf(on) != -1) {
            return 1;
        }
    }
    for (String off : TELECONTROL_OFF) {
        if (cmd.indexOf(off) != -1) {
            return 0;
        }
    }
    return 2;
}

void setup_wifi() {
    WiFiManager manager;
    if (manager.autoConnect("安防报警器")) {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Connect wifi failed...");
    }
}

void uart_handler(void* pvParameters) {
    Serial2.begin(9600);
    while (1) {
        vTaskDelay(1);
        if (Serial2.available() > 0) {
            String cmd = Serial2.readStringUntil('\n');
            Serial.println("Received message: " + cmd);
            int cmdType = get_cmd_type(cmd);
            if (cmdType == 0) {  // 关闭报警
                Serial.println("stop defence");
                stop_alarm();
                write_defence_state(false);
                close_defence();
            } else if (cmdType == 1) {  // 设防
                Serial.println("start defence");
                write_defence_state(true);
                publish_message("{\"defence_state\": true}");
                open_defence();
            } else {  // 传感器报警信号
                Serial.println("trigger alarm");
                start_alarm();
            }
        }
    }
    vTaskDelete(NULL);
}

void setup() {
    Serial.println("setup...");
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // 关闭断电检测
    Serial.begin(115200);
    pinMode(18, OUTPUT);
    init_alarm();
    digitalWrite(18, LOW);
    load_config();
    setup_wifi();
    init_time_config();
    xTaskCreate(uart_handler, "uart_handler", 2048, NULL, 1, NULL);
    xTaskCreate(mqtt_handler, "mqtt_handler", 2048, NULL, 1, NULL);
    digitalWrite(18, HIGH);
    Serial.println("setup end...");
}

void loop() {}

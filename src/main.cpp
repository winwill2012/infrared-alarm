#include <Arduino.h>
#include <WiFi.h>
#include <esp32-hal-log.h>
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
WiFiManager manager;
TimerHandle_t check_wifi_timer;
IPAddress null_address(0, 0, 0, 0);
// 查询指令类型，0：关闭，1：打开，2：传感器信号
int get_cmd_type(String cmd)
{
    for (String on : TELECONTROL_ON)
    {
        if (cmd.indexOf(on) != -1)
        {
            return 1;
        }
    }
    for (String off : TELECONTROL_OFF)
    {
        if (cmd.indexOf(off) != -1)
        {
            return 0;
        }
    }
    return 2;
}

void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print(millis());
    Serial.print(" - WiFi connected: ");
    Serial.println(WiFi.localIP());
    xTimerStop(check_wifi_timer, 0);
}

void onWiFiDisConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print(millis());
    Serial.println(" - WiFi disconnected");
    xTimerStart(check_wifi_timer, 0);
}

void setup_wifi()
{
    WiFi.useStaticBuffers(true);
    manager.autoConnect("安防报警器");
    Serial.print("WiFi connected, ip address: ");
    Serial.println(WiFi.localIP().toString().c_str());
    WiFi.onEvent(onWiFiDisConnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.onEvent(onWiFiConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
}

void uart_handler(void *pvParameters)
{
    Serial2.begin(9600);
    while (1)
    {
        vTaskDelay(1);
        if (Serial2.available() > 0)
        {
            String cmd = Serial2.readStringUntil('\n');
            Serial.println("Received message: " + cmd);
            int cmdType = get_cmd_type(cmd);
            if (cmdType == 0)
            { // 关闭报警
                Serial.println("stop defence");
                stop_alarm();
                write_defence_state(false);
                close_defence();
            }
            else if (cmdType == 1)
            { // 设防
                Serial.println("start defence");
                write_defence_state(true);
                publish_message("{\"defence_state\": true}");
                open_defence();
            }
            else
            { // 传感器报警信号
                Serial.println("trigger alarm");
                start_alarm();
            }
        }
    }
    vTaskDelete(NULL);
}

void check_wifi(TimerHandle_t timer)
{
    Serial.println("-------------------------------");
    Serial.print("Running time: ");
    Serial.print(millis() / 1000);
    Serial.println("s");
    Serial.print("Wifi connected: ");
    Serial.println(WiFi.isConnected());
    Serial.print("Wifi ipaddress: ");
    Serial.println(WiFi.localIP());
    Serial.println("-------------------------------");
    WiFi.begin(manager.getWiFiSSID(), manager.getWiFiPass());
}

void setup()
{
    Serial.println("setup...");
    Serial.begin(115200);
    pinMode(18, OUTPUT);
    init_alarm();
    digitalWrite(18, LOW);
    load_config();
    setup_wifi();
    init_time_config();
    xTaskCreatePinnedToCore(uart_handler, "uart_handler", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(mqtt_handler, "mqtt_handler", 8192, NULL, 1, NULL, 1);

    check_wifi_timer = xTimerCreate("check_wifi", pdMS_TO_TICKS(2000), pdTRUE, nullptr, check_wifi);

    digitalWrite(18, HIGH);
    Serial.println("setup end...");
}

void loop() {}

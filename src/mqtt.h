#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include "ArduinoJson.h"
#include "PubSubClient.h"
#include "alarm.h"
#include "storage.h"

#define MAX_MQTT_MESSAGE_LENGTH 256
#define MQTT_SERVER "gz-3-mqtt.iot-api.com"
#define MQTT_PORT 1883
#define MQTT_USERNAME "bawl6mrpj3mmhnzj"
#define MQTT_PASSWORD "qZljRcdybY"
#define MQTT_SUB_TOPIC "attributes/push"

#define KEY_START_DEFENCE_TIME "start_defence_time"
#define KEY_STOP_DEFENCE_TIME "stop_defence_time"
#define KEY_DEFENCE_STATE "defence_state"
#define QOS (1)

extern StaticJsonDocument<192> doc;
extern WiFiClient espClient;
extern PubSubClient client;

void publish_message(String message);
void mqtt_handler(void *pvParameters);
void mqtt_reconnect();
void on_message_received(char *topic, byte *payload, unsigned int length);
#endif

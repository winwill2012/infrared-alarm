#include "mqtt.h"

StaticJsonDocument<192> doc;
WiFiClient espClient;
PubSubClient client(espClient);
volatile long last_publish_time = 0;

void publish_message(String message) {
    long delta = millis() - last_publish_time;
    if (delta < 6000) {
        vTaskDelay(6000 - delta);
    }
    Serial.print(millis());
    Serial.print(" : upload message: " + message);
    if (client.publish("attributes", message.c_str())) {
        Serial.println("success");
        last_publish_time = millis();
    } else {
        Serial.println("failed");
    }
}

void mqtt_handler(void* pvParameters) {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(on_message_received);
    while (1) {
        vTaskDelay(1);
        if (!client.connected()) {
            mqtt_reconnect();
        }
        client.loop();
    }
    vTaskDelete(NULL);
}

void mqtt_reconnect() {
    while (!client.connected()) {
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD, 0,
                           QOS, 0, 0, 1)) {
            client.subscribe(MQTT_SUB_TOPIC, QOS);
            client.subscribe("attributes/response", QOS);
            Serial.println("connect to mqtt successfully");
        } else {
            Serial.println("reconnect to mqtt...");
            vTaskDelay(1000);
        }
    }
}

void on_message_received(char* topic, byte* payload, unsigned int length) {
    char message[length];
    for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';
    Serial.print("Received message: ");
    Serial.println(message);
    DeserializationError error =
        deserializeJson(doc, message, MAX_MQTT_MESSAGE_LENGTH);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }
    if (doc.containsKey(KEY_START_DEFENCE_TIME)) {
        write_start_defence_time(doc[KEY_START_DEFENCE_TIME]);
    }
    if (doc.containsKey(KEY_STOP_DEFENCE_TIME)) {
        write_stop_defence_time(doc[KEY_STOP_DEFENCE_TIME]);
    }
    if (doc.containsKey(KEY_DEFENCE_STATE)) {
        if (doc[KEY_DEFENCE_STATE] == 0) {
            stop_alarm();
            write_defence_state(false);
            close_defence();
        } else {
            write_defence_state(true);
            open_defence();
        }
    }
}
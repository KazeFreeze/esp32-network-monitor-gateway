#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <MQTT.h>
#include <WiFiClientSecure.h>
#include "secrets.h"

class MqttManager {
public:
    typedef void (*MessageCallback)(String &topic, String &payload);

    MqttManager(MessageCallback callback) : client(2048), onMessageCallback(callback) {}

    void setup() {
        // Set the Root CA for HiveMQ Cloud
        net.setCACert(root_ca);
        // Long keep-alive for stable scanning
        client.setKeepAlive(120);
        client.setOptions(2048, true, 10000);
        client.begin(MQTT_SERVER, MQTT_PORT, net);
        client.onMessage(onMessageCallback);
    }

    void loop() {
        client.loop();
        if (!client.connected()) {
            reconnect();
        }
    }

    bool isConnected() {
        return client.connected();
    }

    void publish(const char* topic, String payload) {
        if (client.connected()) {
            client.publish(topic, payload);
        }
    }

private:
    WiFiClientSecure net;
    MQTTClient client;
    MessageCallback onMessageCallback;
    unsigned long lastReconnectAttempt = 0;

    void reconnect() {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 10000) {
            lastReconnectAttempt = now;
            Serial.print("Connecting to HiveMQ (Secure)... ");
            if (client.connect("ESP32-WOL", MQTT_USER, MQTT_PASS)) {
                Serial.println("connected!");
                client.subscribe(MQTT_TOPIC);
            } else {
                Serial.print("failed, rc=");
                Serial.println(client.lastError());
            }
        }
    }
};

#endif // MQTT_MANAGER_H

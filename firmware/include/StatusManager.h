#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include <Arduino.h>
#include <ESP32Ping.h>
#include <ArduinoJson.h>

class StatusManager {
public:
    struct DeviceStatus {
        bool online;
        float avgTime;
    };

    DeviceStatus checkDevice(const char* ip) {
        Serial.printf("Pinging IP: %s... ", ip);
        bool success = Ping.ping(ip, 3); // Ping 3 times
        
        DeviceStatus status;
        status.online = success;
        status.avgTime = success ? Ping.averageTime() : 0.0f;
        
        if (success) {
            Serial.printf("Online! (Avg: %.2fms)\n", status.avgTime);
        } else {
            Serial.println("Offline.");
        }
        
        return status;
    }

    String createStatusJson(const char* ip, DeviceStatus status) {
        JsonDocument doc;
        doc["ip"] = ip;
        doc["online"] = status.online;
        doc["latency"] = status.avgTime;
        doc["timestamp"] = millis();

        String output;
        serializeJson(doc, output);
        return output;
    }
};

#endif // STATUS_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "WifiManager.h"
#include "MqttManager.h"
#include "WolManager.h"
#include "StatusManager.h"
#include "NetworkScanner.h"

// Forward declarations
void onMqttMessage(String &topic, String &payload);
void onScanResult(String jsonResult);

// Global Managers
WifiManager wifi;
MqttManager mqtt(onMqttMessage);
WolManager wol;
StatusManager status;
NetworkScanner scanner(onScanResult);

void onScanResult(String jsonResult) {
    Serial.printf("[SCAN] Result: %s\n", jsonResult.c_str());
    mqtt.publish("home/wol/scan/result", jsonResult);
}

void onMqttMessage(String &topic, String &payload) {
    Serial.println("\n--- New MQTT Command ---");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        return;
    }

    const char *cmd = doc["cmd"];
    if (!cmd) return;

    if (strcmp(cmd, "wake") == 0) {
        const char *mac = doc["mac"];
        const char *broadcast = doc["broadcast"];
        if (mac) wol.sendWakeOnLan(mac, broadcast);
    } 
    else if (strcmp(cmd, "ping") == 0) {
        const char *ip = doc["ip"];
        if (ip) {
            StatusManager::DeviceStatus res = status.checkDevice(ip);
            mqtt.publish("home/wol/status", status.createStatusJson(ip, res));
        }
    }
    else if (strcmp(cmd, "scan") == 0) {
        Serial.println("Scan command received.");
        if (scanner.startScan()) {
            mqtt.publish("home/wol/scan/result", "{\"event\":\"scan_started\"}");
        } else {
            mqtt.publish("home/wol/scan/result", "{\"event\":\"scan_rejected\",\"reason\":\"already_scanning\"}");
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n[BOOT] Serial started...");
    
    delay(1000);
    Serial.println("[BOOT] Network Monitor Init...");

    Serial.print("[BOOT] WiFi setup... ");
    wifi.setup();
    Serial.println("done.");

    Serial.print("[BOOT] WOL setup... ");
    wol.setup();
    Serial.println("done.");

    Serial.print("[BOOT] MQTT setup... ");
    mqtt.setup();
    Serial.println("done.");
    
    Serial.println("[BOOT] Ready!");
}

void loop() {
    wifi.loop();
    
    if (wifi.isConnected()) {
        mqtt.loop();
        
        if (scanner.isScanning()) {
            static unsigned long lastScanDebug = 0;
            if (millis() - lastScanDebug > 1000) {
                lastScanDebug = millis();
                Serial.println("[SCAN] Probing...");
            }
        }
        scanner.loop();

        // Heartbeat
        static unsigned long lastHeartbeat = 0;
        if (millis() - lastHeartbeat > 60000) {
            lastHeartbeat = millis();
            JsonDocument hb;
            hb["device"] = "ESP32-Gateway";
            hb["scanning"] = scanner.isScanning();
            hb["uptime"] = millis() / 1000;
            hb["rssi"] = WiFi.RSSI();
            String hbStr;
            serializeJson(hb, hbStr);
            mqtt.publish("home/wol/heartbeat", hbStr);
        }
    }
    
    delay(10); 
}

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "secrets.h"

class WifiManager {
public:
    void setup() {
        WiFi.mode(WIFI_STA);
        // Reduce TX power to prevent brownouts
        WiFi.setTxPower(WIFI_POWER_11dBm);

        // Configure static IP
        IPAddress local_IP, gateway, subnet, dns1, dns2;
        
        /*
        if (local_IP.fromString(STATIC_IP) && 
            gateway.fromString(GATEWAY_IP) && 
            subnet.fromString(SUBNET_MASK) &&
            dns1.fromString(DNS_PRIMARY) &&
            dns2.fromString(DNS_SECONDARY)) {
            
            if (!WiFi.config(local_IP, gateway, subnet, dns1, dns2)) {
                Serial.println("STA Failed to configure Static IP");
            } else {
                Serial.println("Static IP configured successfully");
            }
        }
        */

        connect();
    }

    void loop() {
        if (WiFi.status() != WL_CONNECTED) {
            connect();
        }
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

private:
    unsigned long lastAttempt = 0;

    void connect() {
        unsigned long now = millis();
        if (WiFi.status() != WL_CONNECTED && (now - lastAttempt > 10000 || lastAttempt == 0)) {
            lastAttempt = now;
            Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
            WiFi.begin(WIFI_SSID, WIFI_PASS);
        }
    }
};

#endif // WIFI_MANAGER_H

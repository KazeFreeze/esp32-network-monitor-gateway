#ifndef WOL_MANAGER_H
#define WOL_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>

class WolManager {
public:
    WolManager() : wol(udp) {}

    void setup() {
        wol.setRepeat(3, 100);
        updateBroadcastAddress();
    }

    void updateBroadcastAddress() {
        IPAddress broadcast = wol.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
        wol.setBroadcastAddress(broadcast);
    }

    bool sendWakeOnLan(const char* macStr, const char* broadcastStr = nullptr) {
        uint8_t macBytes[6];
        if (!parseMacAddress(macStr, macBytes)) {
            Serial.printf("Error: Invalid MAC address format: %s\n", macStr);
            return false;
        }

        if (broadcastStr && strlen(broadcastStr) > 0) {
            IPAddress broadcast;
            if (broadcast.fromString(broadcastStr)) {
                Serial.printf("Sending WOL to %s via %s\n", macStr, broadcastStr);
                wol.setBroadcastAddress(broadcast);
                wol.sendMagicPacket(macBytes, sizeof(macBytes), 9);
                return true;
            }
        }

        Serial.printf("Sending WOL to %s via local broadcast\n", macStr);
        updateBroadcastAddress(); // Ensure address is current
        wol.sendMagicPacket(macBytes, sizeof(macBytes), 9);
        return true;
    }

private:
    WiFiUDP udp;
    WakeOnLan wol;

    bool parseMacAddress(const char *macStr, uint8_t *macBytes) {
        int values[6];
        const char* format = strchr(macStr, '-') ? "%x-%x-%x-%x-%x-%x" : "%x:%x:%x:%x:%x:%x";

        if (sscanf(macStr, format, &values[0], &values[1], &values[2],
                   &values[3], &values[4], &values[5]) == 6) {
            for (int i = 0; i < 6; i++) {
                macBytes[i] = (uint8_t)values[i];
            }
            return true;
        }
        return false;
    }
};

#endif // WOL_MANAGER_H

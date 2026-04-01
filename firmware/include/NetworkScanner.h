#ifndef NETWORK_SCANNER_H
#define NETWORK_SCANNER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Ping.h>
#include <ArduinoJson.h>
#include <lwip/etharp.h>
#include <lwip/ip_addr.h>
#include <bitset>

class NetworkScanner {
public:
    typedef void (*ResultCallback)(String jsonResult);

    NetworkScanner(ResultCallback callback) : onResult(callback), scanning(false) {}

    bool startScan() {
        if (scanning) {
            Serial.println("Scan already in progress. Ignoring request.");
            return false;
        }
        scanning = true;
        scanIndex = 1;
        devicesFound = 0;
        currentSweep = 0;
        for (int i = 0; i < MAX_SWEEPS; i++) sweepFound[i] = 0;
        foundIps.reset();
        
        IPAddress localIP = WiFi.localIP();
        subnetPrefix = String(localIP[0]) + "." + String(localIP[1]) + "." + String(localIP[2]) + ".";
        
        Serial.printf("\n[SCAN] Starting network scan on %s0/24...\n", subnetPrefix.c_str());
        return true;
    }

    void loop() {
        if (!scanning) return;

        if (scanIndex > 254) {
            currentSweep++;
            if (currentSweep >= MAX_SWEEPS) {
                finishScan();
                return;
            } else {
                scanIndex = 1;
                Serial.printf("[SCAN] Transitioning to Sweep %d (Thorough ICMP Fallback)...\n", currentSweep + 1);
                return; 
            }
        }

        int batchSize = 10;
        int endIndex = min(scanIndex + batchSize, 255);
        
        // 1. Send UDP triggers for the batch (dummy WOL packets to port 9)
        for (int i = scanIndex; i < endIndex; i++) {
            if (foundIps.test(i)) continue;

            String targetIP = subnetPrefix + String(i);
            IPAddress target;
            if (target.fromString(targetIP)) {
                udp.beginPacket(target, 9);
                udp.write(0);
                udp.endPacket();
            }
        }

        // 2. Wait for ARP resolution
        delay(50); 

        // 3. Process the batch
        for (int i = scanIndex; i < endIndex; i++) {
            if (foundIps.test(i)) continue;

            String targetIP = subnetPrefix + String(i);
            String mac = getMacFromArp(targetIP);
            
            // 4. ICMP Fallback ONLY on second sweep
            if (mac == "unknown" && currentSweep > 0) {
                if (Ping.ping(targetIP.c_str(), 1)) {
                    mac = getMacFromArp(targetIP);
                }
            }
            
            if (mac != "unknown") {
                foundIps.set(i);
                sweepFound[currentSweep]++;
                devicesFound++;
                JsonDocument doc;
                doc["ip"] = targetIP;
                doc["mac"] = mac;
                doc["status"] = "online";
                doc["sweep"] = currentSweep + 1;
                
                String output;
                serializeJson(doc, output);
                onResult(output);
            }
        }
        
        scanIndex = endIndex;
    }

    bool isScanning() { return scanning; }

private:
    ResultCallback onResult;
    WiFiUDP udp;
    bool scanning;
    int scanIndex;
    int devicesFound;
    int currentSweep;
    int sweepFound[2];
    const int MAX_SWEEPS = 2;
    std::bitset<256> foundIps;
    String subnetPrefix;

    void finishScan() {
        scanning = false;
        Serial.println("\n--- SCAN SUMMARY ---");
        Serial.printf("Total Devices Found: %d\n", devicesFound);
        Serial.printf("Sweep 1 (UDP-Fast)  : %d devices\n", sweepFound[0]);
        Serial.printf("Sweep 2 (ICMP-Deep) : %d NEW devices\n", sweepFound[1]);
        Serial.println("--------------------\n");
        
        JsonDocument doc;
        doc["event"] = "scan_complete";
        doc["total_found"] = devicesFound;
        doc["sweep1"] = sweepFound[0];
        doc["sweep2"] = sweepFound[1];
        String output;
        serializeJson(doc, output);
        onResult(output);
    }

    String getMacFromArp(String ipStr) {
        ip4_addr_t target_ip;
        ip4addr_aton(ipStr.c_str(), &target_ip);
        
        struct eth_addr* eth_ret;
        ip4_addr_t* ip_ret;
        struct netif* netif_ret;
        
        // Check LwIP ARP table for the MAC address of the IP we just pinged
        for (int i = 0; i < ARP_TABLE_SIZE; i++) {
            if (etharp_get_entry(i, &ip_ret, &netif_ret, &eth_ret)) {
                if (ip_ret->addr == target_ip.addr) {
                    char macBuf[18];
                    snprintf(macBuf, sizeof(macBuf), "%02X:%02X:%02X:%02X:%02X:%02X",
                             eth_ret->addr[0], eth_ret->addr[1], eth_ret->addr[2],
                             eth_ret->addr[3], eth_ret->addr[4], eth_ret->addr[5]);
                    return String(macBuf);
                }
            }
        }
        return "unknown";
    }
};

#endif // NETWORK_SCANNER_H

# ESP32 Wake-on-LAN MQTT API Documentation

This device acts as a secure bridge between HiveMQ Cloud and your local network. It responds to JSON-based commands over MQTT.

## 📡 MQTT Configuration
- **Broker:** HiveMQ Cloud (TLS Port 8883)
- **Command Topic:** `home/wol/command`
- **Result Topic:** `home/wol/scan/result`
- **Status Topic:** `home/wol/status`
- **Heartbeat Topic:** `home/wol/heartbeat`

---

## 🛠 Commands (Send to `home/wol/command`)

### 1. Wake-on-LAN
Wakes up a target computer on the local network.
```json
{
  "cmd": "wake",
  "mac": "AA:BB:CC:DD:EE:FF",
  "broadcast": "255.255.255.255" (optional)
}
```

### 2. Single Device Ping
Checks if a specific IP is online and returns the latency.
```json
{
  "cmd": "ping",
  "ip": "192.168.254.109"
}
```
**Response Topic:** `home/wol/status`
```json
{ "ip": "192.168.254.109", "online": true, "latency": 4.50, "timestamp": 12345 }
```

### 3. Network Scan
Scans the entire `/24` subnet for all connected devices using a **dual-sweep strategy** for maximum accuracy.

#### 🔄 Scanning Strategy
1.  **Sweep 1 (Fast)**: Uses lightweight UDP triggers to find always-on devices (PCs, routers, servers).
2.  **Sweep 2 (Thorough)**: Uses ICMP (Ping) fallback to wake up sleeping devices (iPhones, Androids, Laptops in standby).

```json
{
  "cmd": "scan"
}
```

#### 📡 Response Topic: `home/wol/scan/result`
The scanner streams results as they are found.

-   **Scan Started**: `{"event": "scan_started"}`
-   **Scan Rejected**: `{"event": "scan_rejected", "reason": "already_scanning"}` (Sent if a scan is requested while one is already in progress).
-   **Device Found**: `{"ip": "192.168.254.109", "mac": "2C:F0:5D:57:5C:63", "status": "online", "sweep": 1}`
-   **Scan Complete**: `{"event": "scan_complete", "total_found": 15, "sweep1": 10, "sweep2": 5}`

---

## 💓 Heartbeat (Automatic)
The device reports its own health periodically to `home/wol/heartbeat`.
```json
{
  "device": "ESP32-Gateway",
  "scanning": false,
  "uptime": 3600,
  "rssi": -65,
  "free_heap": 215440
}
```

---

## 🔒 Security
- **Authentication:** TLS/SSL verified with HiveMQ Root CA.
- **Privacy:** `secrets.h` is excluded from git to protect WiFi and MQTT credentials.
- **Resilience:** Non-blocking connection logic. If WiFi/MQTT drops, the device continues to function and auto-reconnects.

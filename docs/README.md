# ESP32 Wake-on-LAN MQTT API Reference

The device acts as a bridge between HiveMQ Cloud and your local network, responding to JSON commands over MQTT.

## MQTT Topics

| Topic | Direction | Purpose |
|---|---|---|
| `home/wol/command` | Inbound | Send commands to the device |
| `home/wol/scan/result` | Outbound | Scan events and discovered devices |
| `home/wol/status` | Outbound | Ping results |
| `home/wol/heartbeat` | Outbound | Periodic device health report |

**Broker:** HiveMQ Cloud, TLS port 8883

---

## Commands

### Wake-on-LAN

Sends a magic packet to wake a target machine on the local network.

```json
{
  "cmd": "wake",
  "mac": "AA:BB:CC:DD:EE:FF",
  "broadcast": "255.255.255.255"
}
```

`broadcast` is optional and defaults to the subnet broadcast address.

---

### Ping

Checks whether a specific IP is reachable and returns latency.

```json
{
  "cmd": "ping",
  "ip": "192.168.1.100"
}
```

Response on `home/wol/status`:

```json
{ "ip": "192.168.1.100", "online": true, "latency": 4.50, "timestamp": 12345 }
```

---

### Network Scan

Scans the local `/24` subnet using a dual-sweep strategy.

```json
{ "cmd": "scan" }
```

**Sweep 1** sends lightweight UDP probes to find always-on devices (PCs, routers, servers).
**Sweep 2** follows up with ICMP pings to catch devices that were in standby (phones, laptops).

Results stream to `home/wol/scan/result` as devices are found:

| Event | Payload |
|---|---|
| Scan started | `{"event": "scan_started"}` |
| Scan already running | `{"event": "scan_rejected", "reason": "already_scanning"}` |
| Device found | `{"ip": "192.168.1.100", "mac": "2C:F0:5D:57:5C:63", "status": "online", "sweep": 1}` |
| Scan complete | `{"event": "scan_complete", "total_found": 15, "sweep1": 10, "sweep2": 5}` |

---

## Heartbeat

The device publishes to `home/wol/heartbeat` on a fixed interval with basic health info.

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

## Security

- TLS/SSL verified against the HiveMQ root CA.
- `secrets.h` is excluded from version control — WiFi and MQTT credentials stay local.
- Connection logic is non-blocking; the device auto-reconnects if WiFi or MQTT drops.

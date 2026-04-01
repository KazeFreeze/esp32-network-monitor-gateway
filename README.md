# ESP32 Wake-on-LAN MQTT Gateway

A secure, low-latency bridge between HiveMQ Cloud (MQTT) and your local network to remotely wake, ping, and scan devices. Designed to integrate seamlessly with automation platforms like n8n or Home Assistant.

## 🚀 Key Features

- **Dual-Sweep Scanner**: A specialized two-pass discovery engine that can detect modern smartphones and laptops even in deep Wi-Fi sleep modes.
- **WOL over MQTT**: Trigger Wake-on-LAN magic packets for any local device via a simple JSON MQTT command.
- **Secure by Default**: Uses TLS 1.2 for all MQTT communications with HiveMQ Cloud.
- **Modular Architecture**: Clean C++ implementation with dedicated managers for WiFi, MQTT, WOL, and Network Scanning.

## 🛠 Hardware & Build

- **Controller**: ESP32 (Support for both 40MHz and 26MHz XTAL boards).
- **Communication**: 2.4GHz WiFi + MQTT (Secure Port 8883).
- **Firmware Framework**: Arduino with PlatformIO.

## 📡 MQTT Interface

The gateway responds to JSON commands on `home/wol/command`. For full documentation, see the [MQTT API Specification](./docs/README.md).

### Quick Example: Network Scan
```json
{ "cmd": "scan" }
```

## 🧠 Development Note

This firmware was developed with the assistance of generative AI for rapid protocol optimization and specialized network discovery algorithms.

## ⚖️ License

MIT License - see [LICENSE](LICENSE) for details.

# ESP32 Wake-on-LAN MQTT Gateway

A bridge between HiveMQ Cloud (MQTT) and your local network to remotely wake, ping, and scan devices. Designed to work with automation platforms like n8n or Home Assistant.

For a full writeup on how this was built, see the [blog post](https://bernardtapiru.com/posts/projects/esp32-n8n-gateway).

## Key Features

- **Dual-Sweep Scanner**: Two-pass discovery that detects smartphones and laptops even when in deep Wi-Fi sleep.
- **WOL over MQTT**: Trigger Wake-on-LAN magic packets for any local device via a simple JSON command.
- **Secure by Default**: TLS 1.2 for all MQTT communications with HiveMQ Cloud.
- **Modular Architecture**: Separate managers for WiFi, MQTT, WOL, and network scanning.

## Hardware & Build

- **Controller**: ESP32 (supports both 40MHz and 26MHz XTAL boards)
- **Communication**: 2.4GHz WiFi + MQTT (port 8883, TLS)
- **Firmware Framework**: Arduino with PlatformIO

## MQTT Interface

The gateway listens for JSON commands on `home/wol/command`. See the [MQTT API Reference](./docs/README.md) for the full command set.

### Quick example

```json
{ "cmd": "scan" }
```

## License

MIT — see [LICENSE](LICENSE) for details.

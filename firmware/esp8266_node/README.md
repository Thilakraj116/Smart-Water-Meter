ESP8266 Node (Outlet)
======================

This example sketch shows the high-level flow for an ESP8266-based outlet node that measures flow and reports readings to the ESP32 master using ESP-NOW.

Notes
-----
- ESP-NOW support on ESP8266 depends on the Arduino core version. Use the ESP8266 Arduino core that provides ESP-NOW APIs or the community `espnow` libraries.
- Replace the placeholder MAC address for the ESP32 master with the actual MAC of your master device.
- The sketch below is a starting template — adapt sensor reading and payload encoding as needed.

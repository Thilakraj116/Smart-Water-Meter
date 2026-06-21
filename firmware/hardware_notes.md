# Hardware Notes — ESP8266 node + ESP32 master + LoRa

Overview
--------
- Each outlet has an `ESP8266` node connected to a flow/pulse sensor.
- The `ESP8266` nodes send periodic readings to an `ESP32` master using ESP-NOW.
- The `ESP32` master aggregates data and relays it over LoRa to a remote gateway or server for long-distance transmission.

Recommended Components
----------------------
- Node: ESP8266 (Wemos D1 Mini or NodeMCU)
- Master: ESP32 Dev Module
- Flow sensor: YF-S201 (hall-effect pulse) or a water meter pulse output
- LoRa: SX1276 / RFM95 module (3.3V) with antenna

Wiring (Node - ESP8266 + YF-S201)
----------------------------------
- VCC (YF-S201) -> 5V (or 3.3V depending on sensor)
- GND -> GND
- Signal -> D2 (GPIO4) or any interrupt-capable pin

Wiring (Master - ESP32 + RFM95)
--------------------------------
- RFM95 NSS -> GPIO5 (example)
- RFM95 MOSI -> GPIO23
- RFM95 MISO -> GPIO19
- RFM95 SCK -> GPIO18
- RFM95 RST -> GPIO14
- RFM95 DIO0 -> GPIO26
- VCC -> 3.3V
- GND -> GND

ESP-NOW notes
--------------
- ESP-NOW works best with all devices on the same Wi-Fi channel. Set channel on ESP32 master and ESP8266 nodes.
- Use a compact binary payload to minimize airtime.
- For many nodes, consider using the ESP32 as a Wi-Fi AP and let nodes join the same channel.

LoRa uplink notes
-----------------
- Aggregate multiple node readings before sending to save airtime and power.
- Use message IDs and simple ACKs for reliability if needed.

Security
--------
- ESP-NOW supports peer registration and encrypted communication. Register only known peers.
- Protect LoRa uplinks with lightweight encryption (e.g., AES) if sending sensitive data.

Next steps
----------
- Add sensor-reading library and accurate pulse-to-volume conversion in `esp8266_node.ino`.
- Add LoRa transmit code to `esp32_master.ino` using your chosen LoRa library.
- Create a simple server receiver to ingest LoRa packets and store them in a database.

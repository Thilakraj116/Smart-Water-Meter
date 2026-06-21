ESP32 Master
============

This folder contains an example ESP32 sketch that receives ESP-NOW messages from outlet nodes,
processes them, and forwards aggregated/individual reports over LoRa for long-range transport.

Dependencies
------------
- Arduino core for ESP32
- LoRa library

Configuration
-------------
- Set SPI pins and LoRa frequency according to your module (e.g., SX1276/78).
- Configure the payload format to match the nodes' encoder.

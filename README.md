# Smart Water Meter

Project: Smart Water Meter

Description
-----------
This project implements a smart water meter system for residential localities. Each house has a water meter connected to all outlets. The system measures water consumption per house, collects all data centrally, and makes it available on users' mobile devices.

Key features
------------
- Per-house water consumption measurement.
- Central collection and comparison of consumption across a locality.
- Leak detection and unwanted-flow detection using consumption patterns.
- Award system: households that use the least water in the locality can be recognized.
- Mobile access: users can view their usage data on their phones.

How it works (high level)
-------------------------
1. Meters attached to outlets record flow and volume data.
2. Meter nodes transmit readings to a central server or gateway.
3. Server aggregates and analyzes data to detect leaks, abnormal flows, and compute per-house totals.
4. Processed data and alerts are pushed to users' mobile apps.
5. Periodic reports allow comparison across households for awarding efficiency.

Next steps
----------
- Add hardware integration notes and protocols (e.g., MQTT/HTTP).
- Provide mobile app screenshots or API docs in `docs/`.
- Implement server-side ingestion and analytics components.

Firmware
--------
Example firmware sketches for the sensor nodes and master are provided in the `firmware/` folder:

- `firmware/esp8266_node/esp8266_node.ino` — template for an ESP8266 outlet node using ESP-NOW to send meter readings to the ESP32 master.
- `firmware/esp32_master/esp32_master.ino` — template for an ESP32 master that receives ESP-NOW packets and can relay aggregated data over LoRa.

See `firmware/hardware_notes.md` for wiring, module recommendations, and integration tips.

Cloud + Web Dashboard
---------------------
This project includes a simple cloud upload flow and realtime web dashboard:

- `firmware/esp32_master/esp32_lora_sender.ino` — now includes optional WiFi + Firebase REST upload. Fill in `WIFI_SSID`, `WIFI_PASS`, `FIREBASE_HOST`, and `FIREBASE_AUTH` before flashing.
- `web/index.html` — a minimal realtime dashboard using Firebase Realtime Database SDK. Open this file in a browser (or host it) and configure the `firebaseConfig` object with your project settings.

Usage notes
-----------
- On the ESP32 master, the code forwards incoming ESP-NOW FlowDataPacket messages over LoRa and posts a copy to the Realtime Database under `/readings`.
- The web dashboard listens to `/readings` and shows live updates.
- For production, secure your Firebase rules and avoid embedding long-lived secrets on devices; use short-lived tokens or an authenticated gateway.

Firmware and examples
---------------------
See the `firmware/` folder for starter sketches:
- `firmware/esp8266_node/esp8266_node.ino` — outlet node template (ESP8266, ESP-NOW)
- `firmware/esp32_master/esp32_master.ino` — master (ESP32) receives ESP-NOW and forwards over LoRa


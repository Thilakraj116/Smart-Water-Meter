/*
  ESP8266 Node (Outlet) - example sketch

  - Reads a flow/pulse sensor (placeholder) and sends meter readings
    to an ESP32 master using ESP-NOW.
  - Replace placeholder sensor code with your actual sensor library.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Replace with your master MAC (if using unicast). Use broadcast if needed.
uint8_t master_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

struct MeterPayload {
  uint32_t node_id;
  uint32_t timestamp;
  float liters;
};

MeterPayload payload;

void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // optional send callback
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onDataSent);

  // Register peer (master)
  esp_now_add_peer(master_mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Initialize sensor / counters here
  payload.node_id = 1; // unique per node
}

void loop() {
  // Read flow sensor and update payload.liters
  // TODO: integrate actual sensor (e.g., YF-S201 flow sensor)
  payload.timestamp = millis() / 1000;
  payload.liters = random(0, 100) / 10.0; // placeholder sample

  esp_now_send(master_mac, (uint8_t *)&payload, sizeof(payload));
  Serial.printf("Sent: node=%u liters=%.2f\n", payload.node_id, payload.liters);
  delay(5000);
}
/*
  ESP8266 Node (outlet)

  Purpose: read a flow sensor (or simulated value) and send periodic reports
  to the ESP32 master using ESP-NOW.

  TODO: install appropriate ESP-NOW support for ESP8266 (Arduino core or library),
  set the master's MAC address below, and wire your flow sensor.
*/

#include <ESP8266WiFi.h>
// ESP-NOW on ESP8266 may require a separate include or library depending on core
// Example placeholders (replace with the correct header for your core):
// #include <espnow.h>

// Replace with your ESP32 master's MAC address (6 bytes)
uint8_t masterMac[6] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};

struct MeterPayload {
  uint32_t houseId;
  uint32_t timestamp;
  float liters;
};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize ESP-NOW here (library-specific)
  // esp_now_init();
  // esp_now_set_self_role(...);
  // esp_now_add_peer(masterMac, ...);

  // Set up sensor pin(s) here
}

void loop() {
  // Read your flow sensor. Here we simulate a reading.
  float liters = random(10, 100) / 10.0; // simulated

  MeterPayload p;
  p.houseId = 1; // set per-node
  p.timestamp = (uint32_t) (millis() / 1000);
  p.liters = liters;

  // Send via ESP-NOW (pseudo-call; replace with real API)
  // esp_now_send(masterMac, (uint8_t*)&p, sizeof(p));

  Serial.printf("Sent: house=%u liters=%.2f\n", p.houseId, p.liters);
  delay(5000);
}

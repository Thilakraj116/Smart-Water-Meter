/*
  ESP8266 Node - ESP-NOW sender

  - Reads a flow/pulse sensor (placeholder) and sends FlowDataPacket
    to an ESP32 master using ESP-NOW.
  - Replace placeholders with your actual sensor code and set the
    master's MAC address before deployment.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Replace with your ESP32 master's MAC address or leave broadcast
uint8_t master_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

typedef struct {
  float flowRate;           // L/min
  unsigned long timestamp;  // ms since boot
  uint8_t sensorStatus;     // 0=OK,1=No flow,2=Error
  uint16_t pulseCount;      // raw pulses
} FlowDataPacket;

FlowDataPacket packet;
uint16_t pulses = 0;

void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // optional: inspect sendStatus
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP8266 ESP-NOW node starting...");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onDataSent);
  esp_now_add_peer(master_mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // initialize sensor hardware / interrupts here
  packet.sensorStatus = 0;
}

void loop() {
  // TODO: replace with actual pulse sensor handling and flow calculation
  pulses += random(0, 5);
  packet.pulseCount = pulses;
  packet.flowRate = (random(50, 150) / 10.0); // fake 5.0 - 15.0 L/min
  packet.timestamp = millis();

  esp_now_send(master_mac, (uint8_t *)&packet, sizeof(packet));
  Serial.printf("Sent -> flow=%.2f L/min pulses=%u\n", packet.flowRate, packet.pulseCount);

  delay(3000);
}

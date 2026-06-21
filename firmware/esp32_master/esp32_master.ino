/*
  ESP32 Master

  Receives ESP-NOW packets from ESP8266 nodes, logs them over Serial,
  and forwards payloads via LoRa (SX127x) for long-range uplink.
*/

#include <WiFi.h>
#include <esp_now.h>
#include <SPI.h>
#include <LoRa.h>

// LoRa pins - update to match your board/module
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

struct MeterPayload {
  uint32_t houseId;
  uint32_t timestamp;
  float liters;
};

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.printf("Received packet from %s, len=%d\n", macStr, len);

  if (len >= (int)sizeof(MeterPayload)) {
    MeterPayload p;
    memcpy(&p, incomingData, sizeof(p));
    Serial.printf("house=%u liters=%.2f ts=%u\n", p.houseId, p.liters, p.timestamp);

    // Forward via LoRa as a compact message (JSON or binary)
    String out = String("{\"house\":") + p.houseId + ",\"liters\":" + String(p.liters) + "}";
    LoRa.beginPacket();
    LoRa.print(out);
    LoRa.endPacket();
  } else {
    Serial.println("Payload too small");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);

  // Initialize LoRa
  SPI.begin();
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
  } else {
    Serial.println("LoRa initialized");
  }
}

void loop() {
  // main loop can perform aggregation, health checks, or sleep management
  delay(1000);
}

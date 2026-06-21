/*
  ESP32 Master - receives ESP-NOW from nodes and forwards via LoRa

  - Receives FlowDataPacket structs over ESP-NOW from ESP8266 nodes.
  - Forwards the binary packet over LoRa so the remote receiver (gateway)
    can parse the same struct.
  - Configure `MASTER_MAC` in node sketches and ensure same LoRa settings
    are used on the receiver side.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <SPI.h>
#include <LoRa.h>
#include <HTTPClient.h>

// LoRa pins (VSPI)
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_CS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_FREQUENCY 433E6

typedef struct {
  float flowRate;           // L/min
  unsigned long timestamp;  // ms since boot
  uint8_t sensorStatus;     // 0=OK,1=No flow,2=Error
  uint16_t pulseCount;      // raw pulses
} FlowDataPacket;

// ----- WiFi + Firebase configuration (fill before flashing) -----
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
// Realtime Database URL (do not include trailing slash), e.g.
// https://your-project-id.firebaseio.com
#define FIREBASE_HOST "https://YOUR_PROJECT_ID.firebaseio.com"
// If your DB requires auth, provide database secret or an auth token
#define FIREBASE_AUTH "YOUR_DATABASE_SECRET_OR_AUTH_TOKEN"
// ----------------------------------------------------------------

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len != sizeof(FlowDataPacket)) {
    Serial.printf("Received invalid len=%d (expected %u)\n", len, (unsigned)sizeof(FlowDataPacket));
    return;
  }

  FlowDataPacket pkt;
  memcpy(&pkt, incomingData, sizeof(pkt));

  Serial.printf("ESP-NOW -> node data: flow=%.2f L/min pulses=%u ts=%lu\n", pkt.flowRate, pkt.pulseCount, pkt.timestamp);

  // Send packet over LoRa as binary blob
  LoRa.beginPacket();
  LoRa.write((uint8_t *)&pkt, sizeof(pkt));
  LoRa.endPacket();
  Serial.println("Forwarded packet over LoRa");

  // Also send to Firebase Realtime Database (optional)
  sendToFirebase(&pkt, mac);
}

// Connect to WiFi
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.printf("Connecting to WiFi '%s'...\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connect failed (continuing without cloud upload)");
  }
}

// Post FlowDataPacket to Firebase Realtime Database using REST API
void sendToFirebase(FlowDataPacket* pkt, const uint8_t* mac) {
  if (strlen(FIREBASE_HOST) == 0) return;
  connectWiFi();
  if (WiFi.status() != WL_CONNECTED) return;

  // Build MAC string
  char macStr[18];
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Build JSON payload
  String json = "{";
  json += "\"mac\":\""; json += macStr; json += "\",";
  json += "\"flowRate\":"; json += String(pkt->flowRate, 2); json += ",";
  json += "\"pulseCount\":"; json += String(pkt->pulseCount); json += ",";
  json += "\"sensorStatus\":"; json += String(pkt->sensorStatus); json += ",";
  json += "\"timestamp\":"; json += String(pkt->timestamp);
  json += "}";

  // Compose URL: FIREBASE_HOST/readings.json?auth=FIREBASE_AUTH
  String url = String(FIREBASE_HOST) + "/readings.json";
  if (strlen(FIREBASE_AUTH) > 0) {
    url += "?auth=";
    url += FIREBASE_AUTH;
  }

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(json);
  if (code > 0) {
    Serial.printf("Firebase POST code=%d\n", code);
  } else {
    Serial.printf("Firebase POST failed, error=%s\n", http.errorToString(code).c_str());
  }
  http.end();
}

void initLoRa() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("LoRa init failed - check wiring and frequency");
    while (1) delay(1000);
  }
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();
  Serial.println("LoRa initialized (sender)");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Master (ESP-NOW -> LoRa) starting...");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);

  initLoRa();
}

void loop() {
  // nothing here; onDataRecv triggers forwarding
  delay(10);
}

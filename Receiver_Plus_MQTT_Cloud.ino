#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>

// LoRa Configuration
#define ss    5
#define rst   14
#define dio0  2
#define LORA_SCK  22
#define LORA_MISO 19
#define LORA_MOSI 23
SPIClass spiLoRa(VSPI);

// WiFi Credentials
const char* ssid = "Riya";
const char* password = "Dhaminii";

// MQTT Broker Credentials (HiveMQ Cloud)
const char* mqtt_server = "337d30812a644b70b65e4cf3ec6eb82b.s1.eu.hivemq.cloud";  // e.g., abc123456789.s2.eu.hivemq.cloud
const int mqtt_port = 8883;
const char* mqtt_user = "kotihivemq";
const char* mqtt_pass = "Koti@2003";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// LoRa packet storage
String loraMessage = "";

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void connectToMQTT() {
  espClient.setInsecure();  // Skip certificate validation (OK for testing)

  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // WiFi and MQTT
  connectToWiFi();
  connectToMQTT();

  // LoRa
  spiLoRa.begin(LORA_SCK, LORA_MISO, LORA_MOSI, ss);
  LoRa.setSPI(spiLoRa);
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring.");
    while (true);
  }

  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Receiver Initialized!");
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    loraMessage = "";
    while (LoRa.available()) {
      loraMessage += (char)LoRa.read();
    }

    Serial.print("Received from LoRa: ");
    Serial.println(loraMessage);

    // Publish to HiveMQ Cloud
    client.publish("esp32/lora/data", loraMessage.c_str());
    Serial.println("Published to MQTT topic");
  }
}

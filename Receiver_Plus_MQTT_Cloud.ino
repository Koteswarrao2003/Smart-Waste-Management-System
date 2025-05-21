#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <MQTT.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
 
// LoRa Configuration
#define ss    5
#define rst   14
#define dio0  2
#define LORA_SCK  22
#define LORA_MISO 19
#define LORA_MOSI 23
SPIClass spiLoRa(VSPI);
 
 
// WiFi credentials
const char* ssid = "Santosh";
const char* password = "Santosh1";
 
// MQTT broker credentials
const char* mqtt_server = "337d30812a644b70b65e4cf3ec6eb82b.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // Secure port
const char* mqtt_user = "kotihivemq";
const char* mqtt_password = "Koti@2003";
 
// MQTT topics
const char* DHTdata = "esp32";
 
// Initialize WiFi and MQTT clients
WiFiClientSecure net;
MQTTClient client;
 
// Global variables
String loraMessage = "";
String LoRaData = "";
String payload = "";
 
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");
  spiLoRa.begin(LORA_SCK, LORA_MISO, LORA_MOSI, ss);
  LoRa.setSPI(spiLoRa);
  // Setup LoRa
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
  Serial.print(".");
  delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
 
  // WiFi and MQTT setup
  setupWiFi();
  net.setInsecure(); // Disable SSL cert verification
  client.begin(mqtt_server, mqtt_port, net);
  client.onMessage(messageReceived);
  connectToMQTT();
}
 
void loop() {
 
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    loraMessage = "";
    while (LoRa.available()) {
      loraMessage += (char)LoRa.read();
    }
 
    Serial.print("Received from LoRa: ");
    Serial.println(loraMessage);
 
    // Parse JSON
    StaticJsonDocument<200> doc;
    // DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, loraMessage);
    if (error) {
      Serial.print("JSON Parse Failed: ");
      Serial.println(error.c_str());
      return;
    }
 
    float distance = doc["distance"];
    payload = "{\"distance\": " + String(distance, 2) + "}";
 
    // Publish every 15 seconds
    static unsigned long lastPublish = 0;
    if (millis() - lastPublish >= 3000) {
      publishSensorData();
      lastPublish = millis();
    }
  }
  client.loop();
}
 
// Function to connect to WiFi
void setupWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
 
// Function to connect to the MQTT broker
void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      // client.subscribe("your/topic", 2); // optional
    } else {
      Serial.print("Failed to connect. State: ");
      Serial.println(client.lastError());
      delay(2000);
    }
  }
}
 
// Function to publish sensor data
void publishSensorData() {
  if (client.publish(DHTdata, payload.c_str())) {
    Serial.println("Published: " + payload);
  } else {
    Serial.println("Failed to publish data");
  }
}
 
// Dummy callback for received MQTT messages
void messageReceived(String &topic, String &payload) {
  Serial.println("Message received on topic: " + topic);
  Serial.println("Payload: " + payload);
}
 
 

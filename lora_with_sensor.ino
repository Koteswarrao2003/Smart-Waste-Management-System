#include <SPI.h>
#include <LoRa.h>

// Ultrasonic Sensor Pins
const int trigPin = 13;
const int echoPin = 27;
#define SOUND_SPEED 0.034

// LoRa Pins
#define ss    5
#define rst   14
#define dio0  2
#define LORA_SCK  22
#define LORA_MISO 19
#define LORA_MOSI 23

SPIClass spiLoRa(VSPI);

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // LoRa SPI Setup
  spiLoRa.begin(LORA_SCK, LORA_MISO, LORA_MOSI, ss);
  LoRa.setSPI(spiLoRa);
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring.");
    while (true);
  }

  LoRa.setSyncWord(0xF3);  // Must match with receiver
  Serial.println("LoRa Transmitter Initialized!");

  delay(3000);  // Give LoRa module time to stabilize
}

void loop() {
  // Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout = 30 ms
  float distance = duration * SOUND_SPEED / 2;

  // Validate reading
  if (duration == 0 || distance < 2 || distance > 100) {
    Serial.println("Invalid or no distance detected. Skipping send.");
    delay(3000);
    return;
  }

  // Prepare JSON only when valid
  String jsonPayload = "{\"distance\": " + String(distance, 2) + "}";

  // Send LoRa packet
  LoRa.beginPacket();
  LoRa.print(jsonPayload);
  LoRa.endPacket();

  Serial.print("Sent JSON over LoRa: ");
  Serial.println(jsonPayload);

  delay(3000);
}

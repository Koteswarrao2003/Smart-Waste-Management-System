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
}

void loop() {
  // Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  float distance = duration * SOUND_SPEED / 2;

  // Print and send
  if (distance > 2 && distance < 100) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    LoRa.beginPacket();
    LoRa.print("Distance: ");
    LoRa.print(distance);
    LoRa.println(" cm");
    LoRa.endPacket();
  } else {
    Serial.println("Out of range or no object detected");
  }

  delay(1000);
}

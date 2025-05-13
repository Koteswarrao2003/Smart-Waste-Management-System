#include <SPI.h>
#include <LoRa.h>

// LoRa Pins
#define ss    5    // LoRa NSS (CS)
#define rst   14   // LoRa RESET
#define dio0  2    // LoRa DIO0

// Custom SPI Pins (VSPI on ESP32)
#define LORA_SCK  22
#define LORA_MISO 19
#define LORA_MOSI 23

// Create SPI object for LoRa
SPIClass spiLoRa(VSPI);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Begin SPI for LoRa
  spiLoRa.begin(LORA_SCK, LORA_MISO, LORA_MOSI, ss);
  LoRa.setSPI(spiLoRa);
  LoRa.setPins(ss, rst, dio0);

  // Initialize LoRa
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check connections.");
    while (true);
  }

  LoRa.setSyncWord(0xF3);  // Must match with transmitter
  Serial.println("✅ LoRa Receiver Initialized!");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) {
    Serial.print("📥 Received packet: ");

    // Read and print the message
    while (LoRa.available()) {
      String message = LoRa.readString();
      Serial.print(message);
    }

    Serial.println();  // New line after the message
  }
}

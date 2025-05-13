#include <SPI.h>
#include <LoRa.h>

// Define LoRa module pins
#define ss 5
#define rst 14
#define dio0 2

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Simulated Sender");

  // Setup LoRa transceiver
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
    Serial.print(".");
    delay(500);
  }

  LoRa.setSyncWord(0xF3);  // Must match receiver
  Serial.println("\nLoRa Initializing OK!");
}

void loop() {
  // Simulate bin level (random number from 0 to 100 cm)
  int binLevel = random(0, 101);

  // Determine bin status
  String status;
  if (binLevel < 20) {
    status = "FULL";
  } else if (binLevel < 60) {
    status = "HALF";
  } else {
    status = "EMPTY";
  }

  // Print to Serial Monitor
  Serial.print("Bin Level: ");
  Serial.print(binLevel);
  Serial.print(" cm - Status: ");
  Serial.println(status);

  // Send data via LoRa
  LoRa.beginPacket();
  LoRa.print("Bin Level: ");
  LoRa.print(binLevel);
  LoRa.print(" cm, Status: ");
  LoRa.print(status);
  LoRa.endPacket();

  delay(3000); 
}

#include <Arduino.h>

// Most ESP32 DevKits have the internal LED on GPIO 2
#define LED_PIN 2

void setup() {
  // 1. Start Serial communication so we can talk to the Mac
  Serial.begin(115200);
  
  // 2. Tell the ESP32 that Pin 2 is an OUTPUT (a "pusher" of electricity)
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("ESP32 System Booted. Starting Blink...");
}

void loop() {
  // 3. Turn the LED ON (High voltage / 3.3V)
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED is ON (Logic H)");
  delay(500); // Wait for 0.5 seconds

  // 4. Turn the LED OFF (Low voltage / 0V)
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED is OFF (Logic L)");
  delay(500); // Wait for 0.5 seconds
}
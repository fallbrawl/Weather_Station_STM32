#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "WeatherProtocol.h"
#include <SPI.h>

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  pinMode(Config::CS_PIN, OUTPUT);
  digitalWrite(Config::CS_PIN, HIGH);
  
  SPI.begin(Config::SCK, Config::MISO, Config::MOSI, Config::CS_PIN); 
  Serial.println("Weather Station Master Active...");

  WiFi.begin(Config::SSID, Config::PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
    WeatherData incoming;
    fetchSpiData(incoming);
    
    JsonDocument doc = get_json(&incoming);
    char output[128];

    send_udp_message(doc, output);

    Serial.print("Sent UDP: ");
    Serial.println(output);

    delay(1000);
}

void fetchSpiData(WeatherData& data) {
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(Config::CS_PIN, LOW);
    
    uint8_t* ptr = (uint8_t*)&data;
    for(size_t i = 0; i < sizeof(WeatherData); i++) {
        ptr[i] = SPI.transfer(0x00);
    }
    
    digitalWrite(Config::CS_PIN, HIGH);
    SPI.endTransaction();
}

JsonDocument get_json(WeatherData* data) {
    JsonDocument doc;

    doc["temp"] = data->temp;
    doc["hum"] = data->hum;
    doc["ts"] = data->ts;

    return doc;
}

void send_udp_message(const JsonDocument& doc, char *output) {
    serializeJson(doc, output, 128);

    udp.beginPacket(Config::DEST_IP, Config::PORT); 
    udp.print(output);
    udp.endPacket();
}
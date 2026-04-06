#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <ArduinoJson.h>

#define CS_PIN 5

const auto ssid = "Lolkek";
const auto password = "happyplum876";
const auto macIp = "192.168.1.241";
const auto port = 1234;
WiFiUDP udp;

struct __attribute__((packed)) WeatherData {
    float temp;
    float hum;
    uint32_t ts;
};

JsonDocument get_json(WeatherData* data);

void setup() {
  Serial.begin(115200);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  
  SPI.begin(18, 19, 23, 5); 
  Serial.println("Weather Station Master Active...");

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
    WeatherData incoming;
    uint8_t* ptr = (uint8_t*)&incoming;

    SPI.beginTransaction(SPISettings(10000, MSBFIRST, SPI_MODE0));
    digitalWrite(CS_PIN, LOW);
    delayMicroseconds(50);

    for(int i = 0; i < sizeof(WeatherData); i++) {
        ptr[i] = SPI.transfer(0x00);
    }

    digitalWrite(CS_PIN, HIGH);
    SPI.endTransaction();
    
    JsonDocument doc = get_json(&incoming);

    char output[128];
    serializeJson(doc, output);

    udp.beginPacket(macIp, port); 
    udp.print(output);
    udp.endPacket();

    Serial.print("Sent UDP: ");
    Serial.println(output);

    delay(1000);
}

JsonDocument get_json(WeatherData* data) {
    JsonDocument doc;

    auto temp = data->temp;
    auto hum = data->hum;
    auto ts = data->ts;

    doc["temp"] = temp;
    doc["hum"] = hum;
    doc["ts"] = ts;

    // Serial.printf("T: %.2f | H: %.2f | TS: %u\n", temp, hum, ts);

    return doc;
}
#ifndef WEATHER_PROTOCOL_H
#define WEATHER_PROTOCOL_H

#include <stdint.h>
#include <ArduinoJson.h>

struct __attribute__((packed)) WeatherData {
    float temp;
    float hum;
    uint32_t ts;
};

namespace Config {
    const char* const SSID = "Lolkek";
    const char* const PASS = "";
    const char* const DEST_IP = "192.168.1.129";
    const uint16_t PORT = 1234;
    
    const uint8_t CS_PIN = 5;
    const uint8_t SCK = 18;
    const uint8_t MISO = 19;
    const uint8_t MOSI = 23;
}

void wifi_init_sta(void);
void init_spi(void);

#endif
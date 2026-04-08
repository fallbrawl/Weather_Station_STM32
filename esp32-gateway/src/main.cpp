#include "WeatherProtocol.h"
#include "driver/spi_master.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN_APP";
spi_device_handle_t spi_handle;

void init_spi() {
    spi_bus_config_t buscfg = {
        .mosi_io_num = Config::MOSI,
        .miso_io_num = Config::MISO,
        .sclk_io_num = Config::SCK,
        .quadwp_io_num = -1, .quadhd_io_num = -1,
    };
    spi_device_interface_config_t devcfg = {
        .mode = 0, .clock_speed_hz = 1000000,
        .spics_io_num = Config::CS_PIN, .queue_size = 7,
    };
    spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle);
}

void weather_task(void *pvParameters) {
    WeatherData data;
    
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(Config::DEST_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(Config::PORT);

    while (1) {
        spi_transaction_t t = {};
        t.length = sizeof(WeatherData) * 8;
        t.rx_buffer = &data;
        spi_device_transmit(spi_handle, &t);

        JsonDocument doc;
        doc["temp"] = data.temp;
        doc["hum"] = data.hum;
        doc["ts"] = data.ts;

        char buffer[128];
        auto n = serializeJson(doc, buffer, sizeof(buffer));

        auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock >= 0) {
            sendto(sock, buffer, n, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            close(sock);
            ESP_LOGI(TAG, "Sent UDP: %s", buffer);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main(void) {
    auto ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();
    init_spi();

    xTaskCreatePinnedToCore(weather_task, "weather_task", 4096, NULL, 5, NULL, 1);
}
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "sdkconfig.h"

#define PIN CONFIG_ESP_DHT_PIN
#define TAG "SENSOR"

int pulseIn(int timeout, u_int8_t state) {
    int64_t ref_time = esp_timer_get_time();
    while (gpio_get_level(PIN) != state) {
        if ((esp_timer_get_time() - ref_time) > timeout)
            return -1;
    }

    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(PIN) == state) {
        if ((esp_timer_get_time() - ref_time) > timeout)
            return -1;
    }

    // ESP_LOGI(TAG, "passou na leitura do pulso");
    return esp_timer_get_time() - start_time;
}

esp_err_t ht_read(float *temperature, float *humidity) {
    u_int8_t bytes[5] = {0};
    int enlapsed = 0;

    ESP_LOGI(TAG, "starting reading");

    gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN, 0);
    ets_delay_us(15000);
    gpio_set_level(PIN, 1);
    gpio_set_direction(PIN, GPIO_MODE_INPUT);

    // do not store the two first reads
    enlapsed = pulseIn(10000, 1);
    if (enlapsed < 0) {
        ESP_LOGE(TAG, "error trying to read the 1st pulse");
        return ESP_FAIL;
    }

    /* enlapsed = pulseIn(10000, 1);
    if (enlapsed < 0) {
        ESP_LOGE(TAG, "error trying to read the 2nd pulse");
        return ESP_FAIL;
    } */

    uint8_t bitIdx = 7;
    uint8_t byteIdx = 0;
    for (u_int8_t i = 0; i < 40; i++) {
        enlapsed = pulseIn(1000, 1);
        if (enlapsed < 0) {
            ESP_LOGE(TAG, "error reading high value. iteration: %d", i);
            return ESP_FAIL;
        }

        if (enlapsed > 45) {
            bytes[byteIdx] |= (1 << bitIdx);
        }

        if (bitIdx == 0) {
            bitIdx = 7;
            byteIdx++;
        } else {
            bitIdx--;
        }
    }

    for (int i = 0; i < 4; i++) {
        ESP_LOGD(TAG, "byte %d, value: %X", i, bytes[i]);
    }

    if (bytes[4] != ((bytes[0] + bytes[1] + bytes[2] + bytes[3]) & 0x00FF)) {
        ESP_LOGE(TAG, "parity check error ");
        return ESP_FAIL;
    }

    *temperature = ((bytes[2] << 8) | bytes[3]) / 10.0;
    *humidity = ((bytes[0] << 8) | bytes[1]) / 10.0;

    return ESP_OK;
}

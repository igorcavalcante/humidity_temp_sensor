#include "dht22.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "MQTT.h"

#define TAG "HUMIDITY"
EventGroupHandle_t conn_state;

void ht_read_task(void *args);
void setup(void);
void app_main(void);

void ht_read_task(void *args) {
  ESP_LOGI(TAG, "init read task");
  float temperature, humidity = 0.0;

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    esp_err_t resp = ht_read(&temperature, &humidity);
    if (resp == ESP_OK) {
      ESP_LOGI(TAG, "humidity: %f", humidity);
      ESP_LOGI(TAG, "temperature: %f", temperature);
      mqtt_send(1);
    }
  }
}

void setup(void) {
  ESP_LOGI(TAG, "running setup");
  conn_state = xEventGroupCreate();
  wifi_init();
  mqtt_setup();
}

void app_main(void) {
  setup();
  xTaskCreate(&ht_read_task, "humidity and temp read task", 5 * 4096, NULL, 5, NULL);
}

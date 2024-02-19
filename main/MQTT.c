#include "MQTT.h"
#include "freertos/projdefs.h"
#include "general.h"

#define TAG "MQTT"
#define TOPIC CONFIG_ESP_MQTT_TOPIC
#define CLIENT_ID CONFIG_ESP_MQTT_CLIENT_ID
#define URI CONFIG_ESP_MQTT_URI
#define PASS CONFIG_ESP_MQTT_PASS
#define USERNAME CONFIG_ESP_MQTT_USERNAME

char retries = 0;
char max_retries = 5;
esp_mqtt_client_handle_t client = NULL;
extern EventGroupHandle_t conn_state;
char mqtt_connected = 0;

void mqtt_event_handler_cb(esp_mqtt_event_handle_t event_data);
static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
void connect();
void mqtt_setup();

void connect() {
  esp_mqtt_client_config_t mqttConfig = {.broker.address.uri = URI,
                                         .credentials.client_id = CLIENT_ID,
                                         .credentials.username = USERNAME,
                                         .credentials.authentication.password = PASS};
  client = esp_mqtt_client_init(&mqttConfig);
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
  esp_mqtt_client_start(client);
}

void disconnect() {
  esp_mqtt_client_unregister_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler);
  esp_mqtt_client_destroy(client);
  mqtt_connected = 0;
}

void mqtt_setup() {
  xEventGroupWaitBits(conn_state, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
  connect();
}

void mqtt_send(int value) {
  if (mqtt_connected) {
    ESP_LOGI(TAG, "connected, sending message");
    char data[50];
    sprintf(data, "%d", value);
    printf("Sending: %d \n", value);
    esp_mqtt_client_publish(client, TOPIC, data, strlen(data), 2, false);
  } else {
    ESP_LOGE(TAG, "not connected, skiping message");
  }
}

static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  mqtt_event_handler_cb(event_data);
}

void mqtt_event_handler_cb(esp_mqtt_event_handle_t event_data) {
  switch (event_data->event_id) {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAG, "connected");
    esp_mqtt_client_subscribe(client, TOPIC, 2);
    retries = 0;
    mqtt_connected = 1;
    break;
  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "disconnected");
    disconnect();
    if (retries <= max_retries) {
      ESP_LOGI(TAG, "trying to connect");
      vTaskDelay(pdMS_TO_TICKS(5000));
      retries++;
      mqtt_setup();
    } else {
      ESP_LOGI(TAG, "max retries excedeed rebooting the system");
      esp_restart();
    }
    break;
  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAG, "subscribed, msg_id=%d", event_data->msg_id);
    break;
  case MQTT_EVENT_UNSUBSCRIBED:
    ESP_LOGI(TAG, "unsubscribed, msg_id=%d", event_data->msg_id);
    break;
  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAG, "published, msg_id=%d", event_data->msg_id);
    break;
  case MQTT_EVENT_DATA:
    ESP_LOGI(TAG, "data read");
    printf("TOPIC=%.*s\r\n", event_data->topic_len, event_data->topic);
    printf("DATA=%.*s\r\n", event_data->data_len, event_data->data);
    break;
  case MQTT_EVENT_ERROR:
    ESP_LOGI(TAG, "GENERAL ERROR");
  default:
    ESP_LOGI(TAG, "ANOTHER EVENT - id:%d", event_data->event_id);
    break;
  }
}

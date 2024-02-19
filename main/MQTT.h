#ifndef __MQTT_H
#define __MQTT_H

#include "cJSON.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include <stdio.h>
#include <string.h>

extern TaskHandle_t mqttTaskHandle;

#define MQTT_CONNECTED BIT2
#define MQTT_PUBLISHED BIT3

void mqtt_setup();
void mqtt_send(int value);
void mqtt_task(void *args);

#endif

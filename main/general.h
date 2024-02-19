#ifndef __GENERAL_H
#define __GENERAL_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT (1 << 0)
#define WIFI_FAIL_BIT (1 << 1)
#define MQTT_ON_BIT (1 << 2)
#define MQTT_OFF_BIT (1 << 3)

extern EventGroupHandle_t conn_state;
#endif

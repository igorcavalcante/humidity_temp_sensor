#ifndef __WIFI_H
#define __WIFI_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT (1 << 0)
#define WIFI_FAIL_BIT (1 << 1)

extern EventGroupHandle_t conn_state;

void wifi_init(void);
#endif

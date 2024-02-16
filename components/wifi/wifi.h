#ifndef __WIFI_H
    #define __WIFI_H

    #include "esp_err.h"
    #include "esp_wifi.h"

    #define WIFI_CONNECTED_BIT BIT0
    #define WIFI_FAIL_BIT      BIT1

    static EventGroupHandle_t s_wifi_event_group;

    void wifi_init(void);
#endif
#ifndef __DHT_H
  #define __DHT_H

  #include "esp_err.h"
  esp_err_t ht_read(float *temperature, float *humidity);
#endif

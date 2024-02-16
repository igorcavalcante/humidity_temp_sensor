idf_component_register(SRCS "wifi.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES main nvs_flash
                      )
idf_component_register(SRCS "esp-idf-ds3231.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_gpio esp_driver_i2c 

)
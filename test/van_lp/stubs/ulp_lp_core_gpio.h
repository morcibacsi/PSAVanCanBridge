#pragma once
#include <stdint.h>
typedef unsigned lp_io_num_t;
uint32_t van_test_cycles(void);
uint32_t ulp_lp_core_gpio_get_level(lp_io_num_t pin);
void ulp_lp_core_gpio_set_level(lp_io_num_t pin, uint8_t level);

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#define LED 2
esp_err_t init_led(void);
esp_err_t blink_led(void);
#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "led.h"

uint8_t led_level = 0;

esp_err_t init_led(void)
{
    gpio_reset_pin(LED); //reseteo el estado del pin a default
    gpio_set_direction(LED,GPIO_MODE_OUTPUT); //seteo para que sea output con pull up
    return ESP_OK;
}

esp_err_t blink_led(void)
{
    led_level = !led_level; //cambio el estado de la variable
    gpio_set_level(LED,led_level); //seteo el nivel del pin
    return ESP_OK;
}
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_mac.h"
#include "wifi_ap.h"
#include "led.h"
#include "web_server.h"
#include "esp_spiffs.h"

#define DELAY 10000

void app_main()
{
  static httpd_handle_t server = NULL;

  init_led(); // configuracion del pin/led

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  ESP_ERROR_CHECK(ret);
  /*esp_vfs_spiffs_conf_t spiffs_conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true};
  ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));*/
  wifi_init_softap(); // Inicio el AP
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
  while (true)
  {
    vTaskDelay(DELAY / portTICK_PERIOD_MS); // delay obligatorio
    blink_led();                            // blinkeo led
  }
}

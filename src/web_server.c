#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include <esp_wifi.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "web_server.h"
#include "led.h"

#define HTTPD_MAX_REQ_HDR_LEN 1024

static const char *TAG = "WEBSERVER";
// static const char *HTML = "HTML";

httpd_uri_t html_uri =
    {
        .uri = "/index",
        .method = HTTP_GET,
        .handler = html_get_handler,
        .user_ctx = NULL};

httpd_uri_t pwm_post = {
    .uri = "/pwm",
    .method = HTTP_POST,
    .handler = pwm_handler,
    .user_ctx = NULL};

esp_err_t html_get_handler(httpd_req_t *req)
{
    extern unsigned char index_start[] asm("_binary_index_html_start");
    extern unsigned char index_end[] asm("_binary_index_html_end");
    size_t index_len = index_end - index_start;
    char indexHtml[index_len];
    memcpy(indexHtml, index_start, index_len);
    httpd_resp_send(req, indexHtml, index_len);
    // free(viewHtml);

    /* ESP_LOGI(HTML, "ABRIENDO EL HTML");
     FILE *html_file = fopen("/spiffs/index.html", "r");
     if (html_file == NULL)
     {
         ESP_LOGE(HTML,"NO SE PUEDO ABRIR EL ARCHIVO HTML");
         return ESP_FAIL;
     }
     else
     {
         ESP_LOGI(HTML, "PUDE ABRIR EL ARCHIVO HTML");
         char line[256];
         while (fgets(line, sizeof(line), html_file))
         {
             ESP_LOGI(HTML, "ENTRE EN EL WHILE");
             // Enviar cada línea del archivo como respuesta HTTP
             httpd_resp_sendstr_chunk(req, line);
         }
         fclose(html_file);
         ESP_LOGI(HTML, "CERRE EL FILE");
     }

     // Finalizar la respuesta HTTP
     httpd_resp_sendstr_chunk(req, NULL);
     ESP_LOGI(HTML, "ENVIO OK");*/
    // httpd_resp_send(req, response_message, strlen(response_message));
    return ESP_OK;
}

esp_err_t pwm_handler(httpd_req_t *req)
{
    // Enviar una respuesta HTTP predeterminada
    esp_err_t error = ESP_OK;
    ESP_LOGI(TAG, "ENTRE AL HANDLER DEL PWM");
    blink_led();
    ESP_LOGI(TAG, "SALGO DEL HANDLER DEL PWM");

    // const char *response = "Hello, World!";
    // error = httpd_resp_send(req, response, strlen(response));
    /*if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "ERROR WHILE SENDING RESPONSE");
    }
    else
    {
        ESP_LOGI(TAG, "RESPONSE SEND SUCCESSFULLY");
    }*/
    return error;
}

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Configuracion por default del server

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        // ESP_LOGI(TAG, "Registering HTML");
        httpd_register_uri_handler(server, &html_uri);
        httpd_register_uri_handler(server, &pwm_post);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

void disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server)
    {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK)
        {
            *server = NULL;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

void connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

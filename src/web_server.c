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
#include "cJSON.h"

static const char *TAG = "WEBSERVER";
static const char *PWM = "PWM";

#define CONFIG_HTTPD_MAX_REQ_HDR_LEN 1024

pwm_t pwm;

void init_pwm(pwm_t *pwm)
{
    pwm->intensidad = -1;
    pwm->ih1.h = -1;
    pwm->ih1.m = -1;
    pwm->fh1.h = -1;
    pwm->fh1.m = -1;
    pwm->ih2.h = -1;
    pwm->ih2.m = -1;
    pwm->fh2.h = -1;
    pwm->fh2.m = -1;
    pwm->ih3.h = -1;
    pwm->ih3.m = -1;
    pwm->fh3.h = -1;
    pwm->fh3.m = -1;
    pwm->ih4.h = -1;
    pwm->ih4.m = -1;
    pwm->fh4.h = -1;
    pwm->fh4.m = -1;
    pwm->option = AUTOMATICO;
    pwm->checkh1 = pdFALSE;
    pwm->checkh2 = pdFALSE;
    pwm->checkh3 = pdFALSE;
    pwm->checkh4 = pdFALSE;
    pwm->dia = pdFALSE;
}

void print_pwm(pwm_t *pwm)
{
    ESP_LOGW(TAG, "%d", pwm->intensidad);
    ESP_LOGW(TAG, "Horario 1: Inicio:%d:%d Final: %d:%d", pwm->ih1.h, pwm->ih1.m, pwm->fh1.h, pwm->fh1.m);
    ESP_LOGW(TAG, "Horario 2: Inicio:%d:%d Final: %d:%d", pwm->ih2.h, pwm->ih2.m, pwm->fh2.h, pwm->fh2.m);
    ESP_LOGW(TAG, "Horario 3: Inicio:%d:%d Final: %d:%d", pwm->ih3.h, pwm->ih3.m, pwm->fh3.h, pwm->fh3.m);
    ESP_LOGW(TAG, "Horario 4: Inicio:%d:%d Final: %d:%d", pwm->ih4.h, pwm->ih4.m, pwm->fh4.h, pwm->fh4.m);
    if (pwm->option == MANUAL)
    {
        ESP_LOGW(TAG, "MANUAL");
    }
    else
    {
        ESP_LOGW(TAG, "AUTOMATICO");
    }
    if (pwm->dia == pdTRUE)
    {
        ESP_LOGW(TAG, "SIMULACION DIA ON");
    }
    else
    {
        ESP_LOGW(TAG, "SIMULACION DIA OFF");
    }
}

void analyze_token(char *token, pwm_t *pwm)
{
    int dh, dm; // unidades y decenas de horas y minutos
    switch (token[0])
    {
    case 'r':
        ESP_LOGI(PWM, "%d", strlen(token));
        if (strlen(token) == 7) // caso de que sea un numero de un solo digito
        {
            pwm->intensidad = atoi(&token[6]);
        }
        else if (strlen(token) == 8) // caso de un numero de dos digitos
        {
            dh = atoi(&token[6]);
            ESP_LOGI(TAG, "%d", dh);
            pwm->intensidad = dh;
        }
        else if (strlen(token) == 9) // caso 100
        {
            pwm->intensidad = 100;
        }
        else
        {
            ESP_LOGE(PWM, "Error en parseo del RANGO");
        }

        break;

    case 'o':
        if (token[9] == 'A')
        {
            pwm->option = AUTOMATICO;
        }
        else if (token[9] == 'M')
        {
            pwm->option = MANUAL;
        }
        else
        {
            ESP_LOGE(PWM, "Erro en parseo del MODO");
        }
        break;
    case 'c':
        switch (token[9]) // me fijo el numero del checkbox
        {
        case '1':
            pwm->checkh1 = pdTRUE;
            break;
        case '2':
            pwm->checkh2 = pdTRUE;
            break;
        case '3':
            pwm->checkh3 = pdTRUE;
            break;
        case '4':
            pwm->checkh4 = pdTRUE;
            break;
        default:
            ESP_LOGE(PWM, "Erro en parseo del CHECKBOX");
            break;
        }
        break;
    case 'i':
        dh = atoi(&token[4]);
        dm = atoi(&token[9]);
        if (token[2] == '1')
        {
            pwm->ih1.h = dh;
            pwm->ih1.m = dm;
        }
        else if (token[2] == '2')
        {
            pwm->ih2.h = dh;
            pwm->ih2.m = dm;
        }
        else if (token[2] == '3')
        {
            pwm->ih3.h = dh;
            pwm->ih3.m = dm;
        }
        else if (token[2] == '4')
        {
            pwm->ih4.h = dh;
            pwm->ih4.m = dm;
        }
        else
        {
            ESP_LOGE(PWM, "Erro en parseo del INICIO DE HORARIO");
        }

        break;
    case 'f':
        dh = atoi(&token[4]);
        dm = atoi(&token[9]);
        if (token[2] == '1')
        {
            pwm->fh1.h = dh;
            pwm->fh1.m = dm;
        }
        else if (token[2] == '2')
        {
            pwm->fh2.h = dh;
            pwm->fh2.m = dm;
        }
        else if (token[2] == '3')
        {
            pwm->fh3.h = dh;
            pwm->fh3.m = dm;
        }
        else if (token[2] == '4')
        {
            pwm->fh4.h = dh;
            pwm->fh4.m = dm;
        }
        else
        {
            ESP_LOGE(PWM, "Erro en parseo del INICIO DE HORARIO");
        }
        break;
    case 's':
        pwm->dia = pdTRUE;
        break;
    default:
    }
}

void parse_pwm(char *buff, pwm_t *pwm)
{
    // el & es el separador de los campos
    ESP_LOGI(PWM, "testeo del parseo");
    char delim[2] = "&";
    char *token;
    token = strtok(buff, delim);
    while (token != NULL)
    {
        analyze_token(token, pwm);
        ESP_LOGI(PWM, "%s", token);
        token = strtok(NULL, delim);
    }
    ESP_LOGI(PWM, "Salgo del parseo");
};

httpd_uri_t index_uri = {
    .uri = "/index",
    .method = HTTP_GET,
    .handler = index_get_handler,
    .user_ctx = NULL};

httpd_uri_t config_uri = {
    .uri = "/config",
    .method = HTTP_GET,
    .handler = config_get_handler,
    .user_ctx = NULL};

httpd_uri_t red_post = {
    .uri = "/red",
    .method = HTTP_POST,
    .handler = red_post_handler,
    .user_ctx = NULL};

httpd_uri_t pwm_post = {
    .uri = "/pwm",
    .method = HTTP_POST,
    .handler = pwm_post_handler,
    .user_ctx = NULL};

httpd_uri_t triac_post = {
    .uri = "/triac",
    .method = HTTP_POST,
    .handler = triac_post_handler,
    .user_ctx = NULL};

httpd_uri_t vegeflor_post = {
    .uri = "/vegeflor",
    .method = HTTP_POST,
    .handler = vegeflor_post_handler,
    .user_ctx = NULL};

httpd_uri_t data_pwm_uri = {
    .uri = "/data_red",
    .method = HTTP_GET,
    .handler = red_data_handler,
    .user_ctx = NULL};

httpd_uri_t data_pwm_uri = {
    .uri = "/data_pwm",
    .method = HTTP_GET,
    .handler = pwm_data_handler,
    .user_ctx = NULL};

httpd_uri_t data_pwm_uri = {
    .uri = "/data_triac",
    .method = HTTP_GET,
    .handler = triac_data_handler,
    .user_ctx = NULL};

httpd_uri_t data_pwm_uri = {
    .uri = "/data_vegeflor",
    .method = HTTP_GET,
    .handler = vegeflor_data_handler,
    .user_ctx = NULL};

//----------HANDLERS PARA LOS HTML------------//
esp_err_t index_get_handler(httpd_req_t *req)
{
    extern unsigned char index_start[] asm("_binary_index_html_start");
    extern unsigned char index_end[] asm("_binary_index_html_end");
    size_t index_len = index_end - index_start;
    char indexHtml[index_len];
    memcpy(indexHtml, index_start, index_len);
    httpd_resp_send(req, indexHtml, index_len);
    return ESP_OK;
}

esp_err_t config_get_handler(httpd_req_t *req)
{
    extern unsigned char config_start[] asm("_binary_config_html_start");
    extern unsigned char config_end[] asm("_binary_config_html_end");
    size_t config_len = config_end - config_start;
    char configHtml[config_len];
    memcpy(configHtml, config_start, config_len);
    httpd_resp_send(req, configHtml, config_len);
    return ESP_OK;
}

//----------HANDLERS PARA LOS POST DE LAS SECCIONES------------//
esp_err_t pwm_post_handler(httpd_req_t *req)
{
    // Enviar una respuesta HTTP predeterminada
    ESP_LOGI(TAG, "ENTRE AL HANDLER DEL PWM");
    char buff[200];
    int ret, remaining = 0;
    init_pwm(&pwm);
    remaining = req->content_len;
    ret = req->content_len;
    ESP_LOGI(TAG, "%d", ret);
    ESP_LOGI(TAG, "%d", remaining);
    if (remaining >= sizeof(buff))
    {
        /* Buffer de datos insuficiente */
        ESP_LOGI(TAG, "PAYLOAD MUY GRANDE");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Payload too large");
        return ESP_FAIL;
    }
    else
    {
        while (remaining > 0)
        {
            /* Leer los datos del formulario */
            ret = httpd_req_recv(req, buff, sizeof(buff)); // en buff se pone lo que estaba en el req y devuelve el numero de bytes que se pasaron al buffer
            if (ret <= 0)                                  // si es 0 o menor es que hubo error
            {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                {
                    /* El tiempo de espera para recibir los datos ha expirado */
                    httpd_resp_send_408(req);
                }
                return ESP_FAIL;
            }
            remaining -= ret; // resto la cantidad que se pasaron para pasar en el siguiente ciclo el resto. aca cuidado porque los esstaria sobrescribiendo
            // Procesar los datos recibidos, por ejemplo, almacenarlos en una variable
        }
        ESP_LOGI(TAG, "%s", buff);
        parse_pwm(buff, &pwm);
        print_pwm(&pwm);
        ESP_LOGI(TAG, "Salgo del PWM HANDLER");

        //  aca irian las funciones de Gaston
        return ESP_OK;
    }
}

esp_err_t red_post_handler(httpd_req_t *req)
{

    return ESP_OK;
}

esp_err_t triac_post_handler(httpd_req_t *req)
{

    return ESP_OK;
}

esp_err_t vegeflor_post_handler(httpd_req_t *req)
{

    return ESP_OK;
}

//----------HANDLERS PARA LEER LOS DATOS------------//
esp_err_t red_data_handler(httpd_req_t *req)
{

    return ESP_OK;
}

esp_err_t pwm_data_handler(httpd_req_t *req)
{
    int value = pwm.intensidad;
    char response[5];
    sprintf(response, "%d", value);
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

esp_err_t triac_data_handler(httpd_req_t *req)
{

    return ESP_OK;
}

esp_err_t vegeflor_data_handler(httpd_req_t *req)
{

    return ESP_OK;
}

//---------FUNCIONES DEL WEBSERVER-------------//

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Configuracion por default del server
    config.stack_size = 16384;
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        // ESP_LOGI(TAG, "Registering HTML");
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &config_uri);
        httpd_register_uri_handler(server, &pwm_post_handler);
        httpd_register_uri_handler(server, &red_post_handler);
        httpd_register_uri_handler(server, &triac_post_handler);
        httpd_register_uri_handler(server, &vegeflor_post_handler);
        httpd_register_uri_handler(server, &red_data_handler);
        httpd_register_uri_handler(server, &pwm_data_handler);
        httpd_register_uri_handler(server, &triac_data_handler);
        httpd_register_uri_handler(server, &vegeflor_data_handler);
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

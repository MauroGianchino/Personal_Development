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
static const char *TRIAC = "TRIAC";
static const char *VEGEFLOR = "VEGEFLOR";
static const char *VERSION = "VERSION";
static const char *HORA = "HORA";

red_t red;

pwm_t pwm;

triac_t triac;

vegeflor_t vegeflor;

version_t version;

hora_t hora;

//----------FUNCIONES------------//
void init_pwm(pwm_t *pwm)
{
    pwm->intensidad = -1;
    pwm->ih1.h = -1;
    pwm->ih1.m = -1;
    pwm->fh1.h = -1;
    pwm->fh1.m = -1;
    pwm->modo = AUTOMATICO;
    pwm->dia = pdFALSE;
}

void init_triac(triac_t *triac)
{
    triac->modo = NO;
    triac->ih1.h = -1;
    triac->ih1.m = -1;
    triac->fh1.h = -1;
    triac->fh1.m = -1;
    triac->ih2.h = -1;
    triac->ih2.m = -1;
    triac->fh2.h = -1;
    triac->fh2.m = -1;
    triac->ih3.h = -1;
    triac->ih3.m = -1;
    triac->fh3.h = -1;
    triac->fh3.m = -1;
    triac->ih4.h = -1;
    triac->ih4.m = -1;
    triac->fh4.h = -1;
    triac->fh4.m = -1;
    triac->checkh1 = pdFALSE;
    triac->checkh2 = pdFALSE;
    triac->checkh3 = pdFALSE;
    triac->checkh4 = pdFALSE;
}

void init_vegeflor(vegeflor_t *vegeflor)
{
    vegeflor->modo = FLOR;
}

void init_red(red_t *red)
{
    memset(red->ID, '\0', sizeof(red->ID));
    strcpy(red->ID, "-");
    memset(red->PASS, '\0', sizeof(red->PASS));
    strcpy(red->PASS, "-");
}

void init_version(version_t *version)
{
    version->hw = -1;

    version->bgs = -1;
}

void init_hora(hora_t *hora)
{
    hora->hr.h = -1;

    hora->hr.m = -1;
}

void print_pwm(pwm_t *pwm)
{
    ESP_LOGW(TAG, "%d", pwm->intensidad);
    ESP_LOGW(TAG, "Horario 1: Encendido:%d:%d Apagado: %d:%d", pwm->ih1.h, pwm->ih1.m, pwm->fh1.h, pwm->fh1.m);
    if (pwm->modo == MANUAL)
    {
        ESP_LOGW(TAG, "MODO MANUAL");
    }
    else
    {
        ESP_LOGW(TAG, "MODO AUTOMATICO");
    }
    if (pwm->dia == pdTRUE)
    {
        ESP_LOGW(TAG, "SIMULACION AMANECER / ATARDECER SI");
    }
    else
    {
        ESP_LOGW(TAG, "SIMULACION AMANECER / ATARDECER NO");
    }
}

void print_triac(triac_t *triac)
{

    ESP_LOGW(TAG, "Horario 1: Inicio:%d:%d Final: %d:%d", triac->ih1.h, triac->ih1.m, triac->fh1.h, triac->fh1.m);
    ESP_LOGW(TAG, "Horario 2: Inicio:%d:%d Final: %d:%d", triac->ih2.h, triac->ih2.m, triac->fh2.h, triac->fh2.m);
    ESP_LOGW(TAG, "Horario 3: Inicio:%d:%d Final: %d:%d", triac->ih3.h, triac->ih3.m, triac->fh3.h, triac->fh3.m);
    ESP_LOGW(TAG, "Horario 4: Inicio:%d:%d Final: %d:%d", triac->ih4.h, triac->ih4.m, triac->fh4.h, triac->fh4.m);
    if (triac->modo == SI)
    {
        ESP_LOGW(TAG, "TRIAC ACTIVADO");
    }
    else if (triac->modo == NO)
    {
        ESP_LOGW(TAG, "TRIAC DESACTIVADO");
    }
    else
    {
        ESP_LOGW(TAG, "TRIAC EN AUTOMATICO");
    }
    if (triac->checkh1 == pdFALSE)
    {
        ESP_LOGW(TAG, "HORARIO 1 DESHABILITADO");
    }
    else
    {
        ESP_LOGW(TAG, "HORARIO 1 HABILITADO");
    }
    if (triac->checkh2 == pdFALSE)
    {
        ESP_LOGW(TAG, "HORARIO 2 DESHABILITADO");
    }
    else
    {
        ESP_LOGW(TAG, "HORARIO 2 HABILITADO");
    }
    if (triac->checkh3 == pdFALSE)
    {
        ESP_LOGW(TAG, "HORARIO 3 DESHABILITADO");
    }
    else
    {
        ESP_LOGW(TAG, "HORARIO 3 HABILITADO");
    }
    if (triac->checkh4 == pdFALSE)
    {
        ESP_LOGW(TAG, "HORARIO 4 DESHABILITADO");
    }
    else
    {
        ESP_LOGW(TAG, "HORARIO 4 HABILITADO");
    }
}

void print_vegeflor(vegeflor_t *vegeflor)
{

    if (vegeflor->modo == VEGE)
    {
        ESP_LOGW(TAG, "MODO VEGETACION");
    }
    else
    {
        ESP_LOGW(TAG, "MODO FLORACION");
    }
}

void print_red(red_t *red)
{

    ESP_LOGW(TAG, "ID:%s", red->ID);

    ESP_LOGW(TAG, "PASS:%s", red->PASS);
}

void print_hora(hora_t *hora)
{

    ESP_LOGW(HORA, "Hora: %d:%d", hora->hr.h, hora->hr.m);
}

void analyze_token_pwm(char *token, pwm_t *pwm)
{
    int dh, dm; // unidades y decenas de horas y minutos
    switch (token[0])
    {
    case 'r': // Parseo intensidad
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

    case 'm':
        if (token[10] == 'A')
        {
            pwm->modo = AUTOMATICO;
        }
        else if (token[10] == 'M')
        {
            pwm->modo = MANUAL;
        }
        else
        {
            ESP_LOGE(PWM, "Erro en parseo del MODO");
        }
        break;
    case 'i':
        dh = atoi(&token[7]);
        dm = atoi(&token[12]);

        pwm->ih1.h = dh;
        pwm->ih1.m = dm;

        break;
    case 'f':
        dh = atoi(&token[7]);
        dm = atoi(&token[12]);

        pwm->fh1.h = dh;
        pwm->fh1.m = dm;

        break;
    case 'O':
        if (token[9] == 'S')
        {
            pwm->dia = pdTRUE;
        }
        else
        {
            pwm->dia = pdFALSE;
        }

        break;
    default:
        break;
    }
}

void analyze_token_triac(char *token, triac_t *triac)
{
    int dh, dm; // unidades y decenas de horas y minutos
    ESP_LOGI(TRIAC, "%d", strlen(token));
    switch (token[0])
    {
    case 'm':                 // Parseo modo
        if (token[12] == 'S') // caso de que sea un numero de un solo digito
        {
            triac->modo = SI;
        }
        else if (token[12] == 'N')
        {
            triac->modo = NO;
        }
        else if (token[12] == 'A')
        {
            triac->modo = AUTOMATICO;
        }
        else
        {
            ESP_LOGE(TRIAC, "Error en parseo del MODO");
        }

        break;
    case 'c':
        if (token[9] == '1')
        {
            triac->checkh1 = pdTRUE;
        }
        else if (token[9] == '2')
        {
            triac->checkh2 = pdTRUE;
        }
        else if (token[9] == '3')
        {
            triac->checkh3 = pdTRUE;
        }
        else if (token[9] == '4')
        {
            triac->checkh1 = pdTRUE;
        }
        else
        {
            ESP_LOGE(TRIAC, "Error en parseo del del CHECKBOX");
        }
        break;

    case 'i':
        dh = atoi(&token[4]);
        dm = atoi(&token[9]);
        if (token[2] == '1')
        {
            triac->ih1.h = dh;
            triac->ih1.m = dm;
        }
        else if (token[2] == '2')
        {
            triac->ih2.h = dh;
            triac->ih2.m = dm;
        }
        else if (token[2] == '3')
        {
            triac->ih3.h = dh;
            triac->ih3.m = dm;
        }
        else if (token[2] == '4')
        {
            triac->ih4.h = dh;
            triac->ih4.m = dm;
        }
        else
        {
            ESP_LOGE(PWM, "Erro en parseo del HORARIO INICIAL");
        }
        break;
    case 'f':
        dh = atoi(&token[4]);
        dm = atoi(&token[9]);
        if (token[2] == '1')
        {
            triac->fh1.h = dh;
            triac->fh1.m = dm;
        }
        else if (token[2] == '2')
        {
            triac->fh2.h = dh;
            triac->fh2.m = dm;
        }
        else if (token[2] == '3')
        {
            triac->fh3.h = dh;
            triac->fh3.m = dm;
        }
        else if (token[2] == '4')
        {
            triac->fh4.h = dh;
            triac->fh4.m = dm;
        }
        else
        {
            ESP_LOGE(PWM, "Erro en parseo del HORARIO FINAL");
        }

        break;
    default:
        break;
    }
}

void parse_pwm(char *buff, pwm_t *pwm)
{
    // el & es el separador de los campos
    ESP_LOGI(PWM, "Testeo del parseo de PWM");
    char delim[2] = "&";
    char *token;
    token = strtok(buff, delim);
    while (token != NULL)
    {
        analyze_token_pwm(token, pwm);
        ESP_LOGI(PWM, "%s", token);
        token = strtok(NULL, delim);
    }
    ESP_LOGI(PWM, "Salgo del parseo");
};

void parse_triac(char *buff, triac_t *triac)
{
    // el & es el separador de los campos
    ESP_LOGI(TRIAC, "Testeo del parseo de TRIAC");
    char delim[2] = "&";
    char *token;
    token = strtok(buff, delim);
    while (token != NULL)
    {
        analyze_token_triac(token, triac);
        ESP_LOGI(TRIAC, "%s", token);
        token = strtok(NULL, delim);
    }
    ESP_LOGI(TRIAC, "Salgo del parseo");
}

void parse_vegeflor(char *buff, vegeflor_t *vegeflor)
{
    // el & es el separador de los campos
    ESP_LOGI(VEGEFLOR, "Testeo del parseo de VEGEFLOR");
    if (buff[14] == 'V')
    {
        vegeflor->modo = VEGE;
    }
    else if (buff[14] == 'F')
    {
        vegeflor->modo = FLOR;
    }
    else
    {
        ESP_LOGE(VEGEFLOR, "Error en parseo del MODO");
    }
    ESP_LOGI(VEGEFLOR, "Salgo del parseo");
};

void parse_red(char *buff, red_t *red)
{
    // el & es el separador de los campos
    ESP_LOGI(TAG, "Testeo del parseo de RED");
    char *e;
    int len = strlen(buff);
    int index;
    int equalIndex = 6;
    e = strchr(buff, '&');
    index = (int)(e - buff);
    int secondEqualIndex = index + 12;
    ESP_LOGW(TAG, "%d", index);
    strncpy(red->ID, buff + equalIndex + 1, index - equalIndex - 1);
    strncpy(red->PASS, buff + secondEqualIndex + 1, len - secondEqualIndex - 1);

    ESP_LOGI(TAG, "Salgo del parseo de RED");
};

void parse_hora(char *buff, hora_t *hora)
{
    // el & es el separador de los campos
    ESP_LOGI(HORA, "Testeo del parseo de HORA");

    hora->hr.h = atoi(&buff[5]);
    hora->hr.m = atoi(&buff[10]);

    ESP_LOGI(HORA, "Salgo del parseo HORA");
};

//----------URL´S------------//
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

httpd_uri_t hora_post = {
    .uri = "/hora",
    .method = HTTP_POST,
    .handler = hora_post_handler,
    .user_ctx = NULL};

httpd_uri_t data_red_uri = {
    .uri = "/data_red",
    .method = HTTP_GET,
    .handler = red_data_handler,
    .user_ctx = NULL};

httpd_uri_t data_pwm_uri = {
    .uri = "/data_pwm",
    .method = HTTP_GET,
    .handler = pwm_data_handler,
    .user_ctx = NULL};

httpd_uri_t data_triac_uri = {
    .uri = "/data_triac",
    .method = HTTP_GET,
    .handler = triac_data_handler,
    .user_ctx = NULL};

httpd_uri_t data_vegeflor_uri = {
    .uri = "/data_vegeflor",
    .method = HTTP_GET,
    .handler = vegeflor_data_handler,
    .user_ctx = NULL};

httpd_uri_t version_data_uri = {
    .uri = "/version",
    .method = HTTP_GET,
    .handler = version_data_handler,
    .user_ctx = NULL};

httpd_uri_t hora_data_uri = {
    .uri = "/data_hora",
    .method = HTTP_GET,
    .handler = hora_data_handler,
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
    char buff[150];
    int ret, remaining = 0;
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
    ESP_LOGI(TAG, "ENTRE AL HANDLER DE LA RED");
    char buff[50];
    int ret, remaining = 0;
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
        parse_red(buff, &red);
        print_red(&red);
        ESP_LOGI(TAG, "Salgo del RED HANDLER");
    }

    return ESP_OK;
}

esp_err_t triac_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "ENTRE AL HANDLER DEL TRIAC");
    char buff[200];
    int ret, remaining = 0;
    remaining = req->content_len;
    ret = req->content_len;
    ESP_LOGI(TAG, "%d", ret);
    ESP_LOGI(TAG, "%d", remaining);
    if (remaining >= sizeof(buff))
    {
        /* Buffer de datos insuficiente */
        ESP_LOGI(TRIAC, "PAYLOAD MUY GRANDE");
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
        parse_triac(buff, &triac);
        print_triac(&triac);
        ESP_LOGI(TAG, "Salgo del TRIAC HANDLER");

        //  aca irian las funciones de Gaston
        return ESP_OK;
    }
}

esp_err_t vegeflor_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "ENTRE AL HANDLER DEL VEGEFLOR");
    char buff[30];
    int ret, remaining = 0;
    remaining = req->content_len;
    ret = req->content_len;
    ESP_LOGI(TAG, "%d", ret);
    ESP_LOGI(TAG, "%d", remaining);
    if (remaining >= sizeof(buff))
    {
        /* Buffer de datos insuficiente */
        ESP_LOGI(VEGEFLOR, "PAYLOAD MUY GRANDE");
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
        parse_vegeflor(buff, &vegeflor);
        print_vegeflor(&vegeflor);
        ESP_LOGI(TAG, "Salgo del VEGEFLOR HANDLER");

        //  aca irian las funciones de Gaston
        return ESP_OK;
    }
}

esp_err_t hora_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "ENTRE AL HANDLER DE LA HORA");
    char buff[30];
    int ret, remaining = 0;
    remaining = req->content_len;
    ret = req->content_len;
    ESP_LOGI(TAG, "%d", ret);
    ESP_LOGI(TAG, "%d", remaining);
    if (remaining >= sizeof(buff))
    {
        /* Buffer de datos insuficiente */
        ESP_LOGI(VEGEFLOR, "PAYLOAD MUY GRANDE");
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
        parse_hora(buff, &hora);
        print_hora(&hora);
        ESP_LOGI(TAG, "Salgo del HORA HANDLER");

        //  aca irian las funciones de Gaston
        return ESP_OK;
    }
}

//----------HANDLERS PARA LEER LOS DATOS------------//
esp_err_t red_data_handler(httpd_req_t *req)
{

    cJSON *json_object = cJSON_CreateObject();

    cJSON_AddStringToObject(json_object, "ID", red.ID);
    cJSON_AddStringToObject(json_object, "PASS", red.PASS);

    char *json_str = cJSON_Print(json_object);
    ESP_LOGI(TAG, "JSON ES: %s", json_str);
    cJSON_Delete(json_object);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);

    return ESP_OK;
}

esp_err_t pwm_data_handler(httpd_req_t *req)
{
    char *modo;
    cJSON *json_object = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_object, "intensidad", pwm.intensidad);
    cJSON_AddNumberToObject(json_object, "ih1h", pwm.ih1.h);
    cJSON_AddNumberToObject(json_object, "ih1m", pwm.ih1.m);
    cJSON_AddNumberToObject(json_object, "fh1h", pwm.fh1.h);
    cJSON_AddNumberToObject(json_object, "fh1m", pwm.fh1.m);
    if (pwm.modo == MANUAL)
    {
        modo = "Manual";
    }
    else
    {
        modo = "Automatico";
    }
    cJSON_AddStringToObject(json_object, "Modo", modo);
    if (pwm.dia == pdTRUE)
    {
        modo = "Si";
    }
    else
    {
        modo = "No";
    }
    cJSON_AddStringToObject(json_object, "DIA", modo);

    char *json_str = cJSON_Print(json_object);
    ESP_LOGI(TAG, "JSON ES: %s", json_str);
    cJSON_Delete(json_object);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);
    return ESP_OK;
}

esp_err_t triac_data_handler(httpd_req_t *req)
{
    char *modo;
    cJSON *json_object = cJSON_CreateObject();
    if (triac.modo == SI)
    {
        modo = "Encendido";
        cJSON_AddStringToObject(json_object, "Modo", modo);
        cJSON_AddStringToObject(json_object, "ih1h", "-");
        cJSON_AddStringToObject(json_object, "ih1m", "-");
        cJSON_AddStringToObject(json_object, "fh1h", "-");
        cJSON_AddStringToObject(json_object, "fh1m", "-");
        //
        cJSON_AddStringToObject(json_object, "ih2h", "-");
        cJSON_AddStringToObject(json_object, "ih2m", "-");
        cJSON_AddStringToObject(json_object, "fh2h", "-");
        cJSON_AddStringToObject(json_object, "fh2m", "-");
        //
        cJSON_AddStringToObject(json_object, "ih3h", "-");
        cJSON_AddStringToObject(json_object, "ih3m", "-");
        cJSON_AddStringToObject(json_object, "fh3h", "-");
        cJSON_AddStringToObject(json_object, "fh3m", "-");
        //
        cJSON_AddStringToObject(json_object, "ih4h", "-");
        cJSON_AddStringToObject(json_object, "ih4m", "-");
        cJSON_AddStringToObject(json_object, "fh4h", "-");
        cJSON_AddStringToObject(json_object, "fh4m", "-");
    }
    else if (triac.modo == NO)
    {
        modo = "Apagado";
        cJSON_AddStringToObject(json_object, "Modo", modo);
        cJSON_AddStringToObject(json_object, "ih1h", "-");
        cJSON_AddStringToObject(json_object, "ih1m", "-");
        cJSON_AddStringToObject(json_object, "fh1h", "-");
        cJSON_AddStringToObject(json_object, "fh1m", "-");
        //
        cJSON_AddStringToObject(json_object, "ih2h", "-");
        cJSON_AddStringToObject(json_object, "ih2m", "-");
        cJSON_AddStringToObject(json_object, "fh2h", "-");
        cJSON_AddStringToObject(json_object, "fh2m", "-");
        //
        cJSON_AddStringToObject(json_object, "ih3h", "-");
        cJSON_AddStringToObject(json_object, "ih3m", "-");
        cJSON_AddStringToObject(json_object, "fh3h", "-");
        cJSON_AddStringToObject(json_object, "fh3m", "-");
        //
        cJSON_AddStringToObject(json_object, "ih4h", "-");
        cJSON_AddStringToObject(json_object, "ih4m", "-");
        cJSON_AddStringToObject(json_object, "fh4h", "-");
        cJSON_AddStringToObject(json_object, "fh4m", "-");
    }
    else
    {
        modo = "Automatico";
        cJSON_AddStringToObject(json_object, "Modo", modo);
        if (triac.checkh1 == pdTRUE)
        {
            cJSON_AddNumberToObject(json_object, "ih1h", triac.ih1.h);
            cJSON_AddNumberToObject(json_object, "ih1m", triac.ih1.m);
            cJSON_AddNumberToObject(json_object, "fh1h", triac.fh1.h);
            cJSON_AddNumberToObject(json_object, "fh1m", triac.fh1.m);
        }
        else
        {
            cJSON_AddStringToObject(json_object, "ih1h", "-");
            cJSON_AddStringToObject(json_object, "ih1m", "-");
            cJSON_AddStringToObject(json_object, "fh1h", "-");
            cJSON_AddStringToObject(json_object, "fh1m", "-");
        }
        if (triac.checkh2 == pdTRUE)
        {
            cJSON_AddNumberToObject(json_object, "ih2h", triac.ih2.h);
            cJSON_AddNumberToObject(json_object, "ih2m", triac.ih2.m);
            cJSON_AddNumberToObject(json_object, "fh2h", triac.fh2.h);
            cJSON_AddNumberToObject(json_object, "fh2m", triac.fh2.m);
        }
        else
        {
            cJSON_AddStringToObject(json_object, "ih2h", "-");
            cJSON_AddStringToObject(json_object, "ih2m", "-");
            cJSON_AddStringToObject(json_object, "fh2h", "-");
            cJSON_AddStringToObject(json_object, "fh2m", "-");
        }
        if (triac.checkh3 == pdTRUE)
        {
            cJSON_AddNumberToObject(json_object, "ih3h", triac.ih3.h);
            cJSON_AddNumberToObject(json_object, "ih3m", triac.ih3.m);
            cJSON_AddNumberToObject(json_object, "fh3h", triac.fh3.h);
            cJSON_AddNumberToObject(json_object, "fh3m", triac.fh3.m);
        }
        else
        {
            cJSON_AddStringToObject(json_object, "ih3h", "-");
            cJSON_AddStringToObject(json_object, "ih3m", "-");
            cJSON_AddStringToObject(json_object, "fh3h", "-");
            cJSON_AddStringToObject(json_object, "fh3m", "-");
        }
        if (triac.checkh4 == pdTRUE)
        {
            cJSON_AddNumberToObject(json_object, "ih4h", triac.ih4.h);
            cJSON_AddNumberToObject(json_object, "ih4m", triac.ih4.m);
            cJSON_AddNumberToObject(json_object, "fh4h", triac.fh4.h);
            cJSON_AddNumberToObject(json_object, "fh4m", triac.fh4.m);
        }
        else
        {
            cJSON_AddStringToObject(json_object, "ih4h", "-");
            cJSON_AddStringToObject(json_object, "ih4m", "-");
            cJSON_AddStringToObject(json_object, "fh4h", "-");
            cJSON_AddStringToObject(json_object, "fh4m", "-");
        }
    }

    char *json_str = cJSON_Print(json_object);
    ESP_LOGI(TAG, "JSON ES: %s", json_str);
    cJSON_Delete(json_object);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);

    return ESP_OK;
}

esp_err_t vegeflor_data_handler(httpd_req_t *req)
{
    char *modo;
    cJSON *json_object = cJSON_CreateObject();
    if (vegeflor.modo == VEGE)
    {
        modo = "Vegetacion";
    }
    else
    {
        modo = "Floracion";
    }
    cJSON_AddStringToObject(json_object, "Modo", modo);

    char *json_str = cJSON_Print(json_object);
    ESP_LOGI(TAG, "JSON ES: %s", json_str);
    cJSON_Delete(json_object);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);

    return ESP_OK;
}

esp_err_t version_data_handler(httpd_req_t *req)
{
    cJSON *json_object = cJSON_CreateObject();

    cJSON_AddNumberToObject(json_object, "Hardware", version.hw);
    cJSON_AddNumberToObject(json_object, "Bugs", version.bgs);

    char *json_str = cJSON_Print(json_object);
    ESP_LOGI(TAG, "JSON ES: %s", json_str);
    cJSON_Delete(json_object);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);

    return ESP_OK;
}

esp_err_t hora_data_handler(httpd_req_t *req)
{
    cJSON *json_object = cJSON_CreateObject();

    cJSON_AddNumberToObject(json_object, "Hora_h", hora.hr.h);
    cJSON_AddNumberToObject(json_object, "Hora_m", hora.hr.m);

    char *json_str = cJSON_Print(json_object);
    ESP_LOGI(TAG, "JSON ES: %s", json_str);
    cJSON_Delete(json_object);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);

    return ESP_OK;
}

//---------FUNCIONES DEL WEBSERVER-------------//

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Configuracion por default del server
    config.stack_size = 16384;
    config.max_uri_handlers = 15;
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        // ESP_LOGI(TAG, "Registering HTML");
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &config_uri);
        httpd_register_uri_handler(server, &pwm_post);
        httpd_register_uri_handler(server, &red_post);
        httpd_register_uri_handler(server, &triac_post);
        httpd_register_uri_handler(server, &vegeflor_post);
        httpd_register_uri_handler(server, &data_red_uri);
        httpd_register_uri_handler(server, &data_pwm_uri);
        httpd_register_uri_handler(server, &data_triac_uri);
        httpd_register_uri_handler(server, &data_vegeflor_uri);
        httpd_register_uri_handler(server, &version_data_uri);
        httpd_register_uri_handler(server, &hora_data_uri);
        httpd_register_uri_handler(server, &hora_post);
        ESP_LOGI("RED", "INICIO VARIABLE RED");
        init_red(&red);
        ESP_LOGI(PWM, "INICIO VARIABLE PWM");
        init_pwm(&pwm);
        ESP_LOGI(TRIAC, "INICIO VARIABLE TRIAC");
        init_triac(&triac);
        ESP_LOGI(VEGEFLOR, "INICIO VARIABLE VEGEFLOR");
        init_vegeflor(&vegeflor);
        ESP_LOGI(VERSION, "INICIO VARIABLE VERSION");
        init_version(&version);
        ESP_LOGI(HORA, "INICIO VARIABLE HORA");
        init_hora(&hora);
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

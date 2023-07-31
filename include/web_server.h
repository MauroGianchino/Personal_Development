#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include <esp_wifi.h>
#include <esp_system.h>
#include "nvs_flash.h"

typedef enum
{
    MANUAL,
    AUTOMATICO,
    SI,
    NO,
    VEGE,
    FLOR
} modo_t;

typedef struct horario
{
    int h;
    int m;

} horario_t;

typedef struct red
{
    char ID[20];
    char PASS[20];
} red_t;

typedef struct pwm
{
    int intensidad;
    horario_t ih1;
    horario_t fh1;
    modo_t modo;
    bool dia;
} pwm_t;

typedef struct triac
{
    modo_t modo;
    horario_t ih1;
    horario_t fh1;
    horario_t ih2;
    horario_t fh2;
    horario_t ih3;
    horario_t fh3;
    horario_t ih4;
    horario_t fh4;
    bool checkh1;
    bool checkh2;
    bool checkh3;
    bool checkh4;

} triac_t;

typedef struct vegeflor
{
    modo_t modo;
} vegeflor_t;

typedef struct version
{
    int hw;
    int bgs;
} version_t;

typedef struct hora
{
    horario_t hr;
} hora_t;

esp_err_t index_get_handler(httpd_req_t *);
esp_err_t config_get_handler(httpd_req_t *);
esp_err_t pwm_post_handler(httpd_req_t *);
esp_err_t red_post_handler(httpd_req_t *);
esp_err_t triac_post_handler(httpd_req_t *);
esp_err_t vegeflor_post_handler(httpd_req_t *);
esp_err_t hora_post_handler(httpd_req_t *);
esp_err_t red_data_handler(httpd_req_t *);
esp_err_t pwm_data_handler(httpd_req_t *);
esp_err_t triac_data_handler(httpd_req_t *);
esp_err_t vegeflor_data_handler(httpd_req_t *);
esp_err_t version_data_handler(httpd_req_t *);
esp_err_t hora_data_handler(httpd_req_t *);
httpd_handle_t start_webserver(void);
esp_err_t stop_webserver(httpd_handle_t);
void disconnect_handler(void *, esp_event_base_t, int32_t, void *);
void connect_handler(void *, esp_event_base_t, int32_t, void *);
void parse_pwm(char *, pwm_t *);
void parse_triac(char *, triac_t *);
void parse_vegeflor(char *, vegeflor_t *);
void parse_red(char *buff, red_t *);
void analyze_token_pwm(char *, pwm_t *);
void analyze_token_triac(char *, triac_t *);
void init_pwm(pwm_t *);
void init_triac(triac_t *);
void init_vegeflor(vegeflor_t *);
void init_red(red_t *);
void init_version(version_t *);
void init_hora(hora_t *);
void print_pwm(pwm_t *);
void print_triac(triac_t *);
void print_vegeflor(vegeflor_t *);
void print_red(red_t *);
void print_hora(hora_t *);
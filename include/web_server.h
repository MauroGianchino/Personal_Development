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
    AUTOMATICO
} option_t;

typedef struct horario
{
    int h;
    int m;

} horario_t;
typedef struct pwm
{
    int intensidad;
    horario_t ih1;
    horario_t fh1;
    horario_t ih2;
    horario_t fh2;
    horario_t ih3;
    horario_t fh3;
    horario_t ih4;
    horario_t fh4;
    option_t option;
    bool checkh1;
    bool checkh2;
    bool checkh3;
    bool checkh4;
    bool dia;
} pwm_t;

esp_err_t html_get_handler(httpd_req_t *);
esp_err_t pwm_handler(httpd_req_t *);
httpd_handle_t start_webserver(void);
esp_err_t stop_webserver(httpd_handle_t);
void disconnect_handler(void *, esp_event_base_t, int32_t, void *);
void connect_handler(void *, esp_event_base_t, int32_t, void *);
void parse_pwm(char *, pwm_t *);
void analyze_token(char *, pwm_t *);
void init_pwm(pwm_t *);
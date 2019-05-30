#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "driver/ws2812b.h"
#include "wordclock.h"
#include "httpserver_raw/httpd_callbacks.h"

#include <httpd/httpd.h>

#include <sntp.h>
#include <time.h>

#include <string.h>

#include "ssid_config.h"

#define SNTP_SERVERS "0.pool.ntp.org", "1.pool.ntp.org", \
                                    "2.pool.ntp.org", "3.pool.ntp.org"

#define vTaskDelayMs(ms) vTaskDelay((ms)/portTICK_PERIOD_MS)

static TaskHandle_t getSNTPTaskHandle;

void readNTPTime(void* pvParameters)
{
    /* Wait until we have joined AP and are assigned an IP */
    while (sdk_wifi_station_get_connect_status() != STATION_GOT_IP) {
        vTaskDelayMs(1000);
        printf("%s\n", "Connecting..");
    }

    const char *servers[] = {SNTP_SERVERS};

    /* SNTP will request an update each 5 minutes */
    sntp_set_update_delay(15000);
    /* Set GMT+1 zone, daylight savings middle european style*/
    const struct timezone tz = {0*60, 0};
    //Central European Timezone + Daylight Saving
    putenv("TZ=CET-1CEST,M3.5.0/2,M10.5.0/3");
    /* SNTP initialization */
    sntp_initialize(&tz);
    /* Servers must be configured right after initialization */
    sntp_set_servers(servers, sizeof(servers) / sizeof(char*));

    /* Print date and time each 5 seconds */
    while(1) {
        vTaskDelayMs(5000);
        time_t current_time = time(NULL);
        //printf("TIME: %s", ctime(&current_time));
        wordclock_show(current_time);
    }
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
        .bssid_set = 0,
    };

    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    struct ip_info info;
    info.ip.addr = ipaddr_addr("192.168.0.8");
    info.gw.addr = ipaddr_addr("192.168.0.1");
    info.netmask.addr = ipaddr_addr("255.255.255");

    sdk_wifi_set_ip_info(STATION_IF, &info);

    ws2812b_init();
    wordclock_init();

    //Set Foreground Color to green
    struct rgb fg = {0x0B, 0x28, 0x0B};
    wordclock_set_fg_color(&fg);
    struct rgb bg = {0x00, 0x00, 0x00};
    wordclock_set_bg_color(&bg);

    vTaskDelay((5 * 1000) / portTICK_PERIOD_MS);

    //Initialize HTTP-Server to set colors etc.
    httpd_init_cgi_handler();
    httpd_init_ssi_handler();
    httpd_init();

    xTaskCreate(readNTPTime, "NTPTask", 1024, NULL, 1, &getSNTPTaskHandle);
}
#include "espressif/esp_common.h"
#include "esp/uart.h"
//#include "driver/spi.h"
#include "FreeRTOS.h"
#include "sntp.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "driver/ws2812b.h"
#include "wordclock.h"
#include "httpserver_raw/httpd.h"
#include "httpserver_raw/httpd_callbacks.h"

#include <string.h>

#include <time.h>

#include "ssid_config.h"

//current_time is set by sntp and increased every second by TimerCallback_1s
static time_t current_time = 0;

static xQueueHandle mainqueue;
static xTaskHandle getSNTPTaskHandle;
//static xTaskHandle printSNTPTaskHandle;

static xSemaphoreHandle currentTimeSemaphore;

void readNTPTime(void* pvParameters)
{

    xQueueHandle* queue = (xQueueHandle*)pvParameters;
    time_t new_sntp_time = 0;
    time_t old_sntp_time = 0;

    portTickType xLastWakeTime;
    const portTickType xFrequency = (5 * 1000) / portTICK_RATE_MS;

    //printf("%s\n", "call sntp_init()");
    sntp_init();

    xLastWakeTime = xTaskGetTickCount();

    while(1) {

        //sdk_wifi_set_opmode(STATION_MODE);
        //sdk_wifi_station_connect();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        new_sntp_time = sntp_gettime();
        //printf("%i\n", (int) new_sntp_time);
        if(new_sntp_time != 0 && new_sntp_time != old_sntp_time) {
            old_sntp_time = new_sntp_time;
            if(currentTimeSemaphore != NULL) {
                if(xSemaphoreTake(currentTimeSemaphore, 10 / portTICK_RATE_MS) == pdTRUE) {
                    current_time = new_sntp_time;
                    //printf("Take Semaphor NTP\n");
                    xSemaphoreGive(currentTimeSemaphore);
                }
            }
            xQueueSend(*queue, &new_sntp_time, 0);
        }

        //sdk_wifi_station_disconnect();
        //sdk_wifi_set_opmode(NULL_MODE);
    }
}

// void printTime(void* pvParameters)
// {
//     xQueueHandle* queue = (xQueueHandle*)pvParameters;
//     time_t new_sntp_time = 0;

//     while(1) {
//         if(xQueueReceive(*queue, &new_sntp_time, 100 / portTICK_RATE_MS)) {
//             printf("Aktuelle Zeit: %s\n", ctime(&new_sntp_time));
//         }
//     }
// }

void TimerCallback_1s(xTimerHandle xTHandle)
{
    if(currentTimeSemaphore != NULL) {
        if(xSemaphoreTake(currentTimeSemaphore, 100 / portTICK_RATE_MS) == pdTRUE) {
            struct tm* tm;
            tm = localtime(&current_time);
            tm->tm_sec += 1;
            current_time = mktime(tm);
            //printf("%s\n", ctime(&current_time));
            //sdk_wifi_station_disconnect();
            //vTaskDelay(500 / portTICK_RATE_MS);
            wordclock_show(current_time);
            //sdk_wifi_station_connect();
            //printf("Take Semaphor Timer\n");
            xSemaphoreGive(currentTimeSemaphore);
        }
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

    //sdk_wifi_set_phy_mode(PHY_MODE_11B);
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    //sdk_wifi_set_sleep_type(WIFI_SLEEP_NONE);

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

    //Central European Timezone + Daylight Saving
    putenv("TZ=CET-1CEST,M3.5.0/2,M10.5.0/3");

    vTaskDelay((5 * 1000) / portTICK_RATE_MS);

    //Initialize HTTP-Server to set colors etc.
    httpd_init_cgi_handler();
    httpd_init_ssi_handler();
    httpd_init();

    vSemaphoreCreateBinary(currentTimeSemaphore);

    if(currentTimeSemaphore == NULL) {
        printf("%s\n", "Error: Failed to create semaphore!");
    }

    mainqueue = xQueueCreate(1, sizeof(time_t));
    xTaskCreate(readNTPTime, "NTPTask", 256, &mainqueue, 1, &getSNTPTaskHandle);
    //xTaskCreate(printTime, (signed char*)"PrintTask", 256, &mainqueue, 2, &printSNTPTaskHandle);


    xTimerHandle Timer_1s_Handle;
    Timer_1s_Handle = xTimerCreate("Timer_1s", (1000 / portTICK_RATE_MS), pdTRUE, NULL, TimerCallback_1s);
    xTimerStart(Timer_1s_Handle, 0);

}
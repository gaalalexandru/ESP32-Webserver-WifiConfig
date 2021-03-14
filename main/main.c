//main.c
/*Inclusion of system header files*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"

/*Inclusion of application header files*/
#include "config.h"
#include "storage_handler.h"
#include "wifi_handler.h"
#include "webserver_handler.h"

/*************************************************
 * Global Data Definition
 *************************************************/
extern bool WIFI_boHwInitDone;

/*************************************************
 * File Local Data Definition
 ************************************************/
static uint8_t u8WifiTaskPeriod = 100;
static uint8_t u8HttpSrvTaskPeriod = 50;
static const char *MAIN_TAG = "MAIN";

/*************************************************
 * File Local Function Definition and Prototipes
 ************************************************/

/*************************************************
 * Function Implementation (External and Local)
 ************************************************/
void app_main()
{   
    /* Initialize Factory NVS — it is used to read fectory / production data data */
    ESP_LOGI(MAIN_TAG, "Initializing NonVolatile Storage");
    /* Initialize User NVS — it is used to store user configuration data */
    storage_errInitUserNvs();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
    /* Initialize SPIFFS — it is used to store the webpage data that is needed for device configuration*/
    ESP_LOGI(MAIN_TAG, "Initializing SPIFFS");
    
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 10,
      .format_if_mount_failed = true
    };
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(MAIN_TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(MAIN_TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(MAIN_TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(MAIN_TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(MAIN_TAG, "Partition size: total: %d, used: %d", total, used);
    }

    ESP_LOGI(MAIN_TAG, "Storages initialized, starting WiFi and Webserver tasks");

    vTaskDelay(10 / portTICK_PERIOD_MS);

    xTaskCreatePinnedToCore(vTaskWifiHandler, "WiFi Handler task", 10*1024, &u8WifiTaskPeriod, 2, NULL, 0);
    while(WIFI_boHwInitDone == false)
    {
        //wait here untill TCP IP adaptor is initialized so that we can initialize Http server
    }
    xTaskCreatePinnedToCore(vTaskHttpServer, "HTTP - Webserver task", 20*1024, &u8HttpSrvTaskPeriod, 6, NULL, 0); // start the HTTP Server task
}
//EOF
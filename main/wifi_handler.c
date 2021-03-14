/* System header files*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include <esp_event.h>
#include "nvs_flash.h"

/* own app header files*/
#include "config.h"
#include "wifi_handler.h"
#include "storage_handler.h"

/*************************************************
 * Global Data Definition
 *************************************************/
char WIFI_acIPAddress[4] = {0,0,0,0};
bool WIFI_boConnected = false;
bool WIFI_boHwInitDone = false;
/*************************************************
 * File Local Data Definition
 ************************************************/
#define TAG __FUNCTION__
//static char TAG[50] = "WiFi - ";

// Event group
EventGroupHandle_t wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int CONNECTION_FAILED_BIT = BIT1;

typedef enum
{
    enSoftAPMode = 0,
    enStationMode,
    enCombinedMode,
    enNrOfModes
} tenWifiModes;

/*************************************************
 * File Local Function Definition and Prototipes
 ************************************************/
static void WIFI_vInitCommon(void);
static void WIFI_vMainFunc(void);
static void WIFI_vConfigSoftAP(void);
static void WIFI_vConfigStation(void);
static void WIFI_vEventHandler(void* arg, esp_event_base_t event_base, int event_id, void* event_data);
static tenWifiModes WIFI_enCheckProvisioningData(void);

/*************************************************
 * Function Implementation (External and Local)
 ************************************************/
static void WIFI_vInitCommon(void)
{
	// Set the default wifi logging
	//esp_log_level_set("wifi", ESP_LOG_ERROR);
    //esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("wifi", ESP_LOG_INFO);

	/* Initialize TCP/IP 
	 * Alternative for ESP_ERROR_CHECK(esp_netif_init()); 
	 */
	tcpip_adapter_init();

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	/* Create the event group to handle wifi events */
    wifi_event_group = xEventGroupCreate();

    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_vEventHandler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WIFI_vEventHandler, NULL));

	/* Initialize Wi-Fi */
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
}

void vTaskWifiHandler( void * pvParameters )
{
    tenWifiModes enWifiMode = WIFI_enCheckProvisioningData();

    ESP_LOGI(TAG,"Starting Wifi Task with %d ms period!", (*(uint8_t*)pvParameters));

    /* Initializing Wi-Fi Station / AccessPoint*/
    WIFI_vInitCommon();
    WIFI_boHwInitDone = true;
    switch(enWifiMode)
    {
        case enStationMode:
            ESP_LOGI(TAG,"Wifi Station initializing!");
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            WIFI_vConfigStation();
        break;

        case enSoftAPMode:
            ESP_LOGI(TAG,"Wifi AccessPoint initializing!");
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
            WIFI_vConfigSoftAP();
        break;

        default:
            ESP_LOGE(TAG, "Unsuported Wifi Mode: %d", enWifiMode);
        break;
    }

	/* Start Wi-Fi Station / AccessPoint*/
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(TAG, "Main task: waiting for connection to the wifi network... ");

    for(;;)
	{
        if(enWifiMode == enStationMode)
        {
            // wait for connection or disconnect event
            EventBits_t bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | CONNECTION_FAILED_BIT, false, false, portMAX_DELAY);
            if (bits & CONNECTED_BIT)
            {
                //printf("Connected!\n");
                WIFI_boConnected = true;
            } 
            else if (bits & CONNECTION_FAILED_BIT) 
            {
                WIFI_boConnected = false;
                xEventGroupClearBits(wifi_event_group, CONNECTION_FAILED_BIT);
                xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
                //If connection is lost clear the provisioning available flag and stop wifi and re-start in AccessPoint Mode
                ESP_LOGI(TAG, "Connection lost! Stopping Wifi to switch to AccessPoint Mode.");
                ESP_ERROR_CHECK(esp_wifi_stop());
                storage_errWriteProvisioningAvailableFlag(0x00);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
                WIFI_vConfigSoftAP();
                enWifiMode = enSoftAPMode;
                ESP_ERROR_CHECK(esp_wifi_start());
            }
            else 
            {
                ESP_LOGE(TAG, "Unexpected event");
            }
        }
        else if(enWifiMode == enSoftAPMode)
        {
            enWifiMode = WIFI_enCheckProvisioningData();
            ESP_LOGV(TAG,"Wifi Mode: %d",enWifiMode);
            //received new Provisioning Data, but not tested yet.
            if(enWifiMode == enStationMode)
            {
                ESP_LOGI(TAG, "New Provisioning data received. Stopping Wifi to switch to Station Mode.");
                ESP_ERROR_CHECK(esp_wifi_stop());
                ESP_LOGI(TAG,"Wifi Station initializing!");
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                WIFI_vConfigStation();
                ESP_ERROR_CHECK(esp_wifi_start());
            }
            else
            {
                /*Do nothing*/
                ESP_LOGV(TAG,"Wifi AP");
            }
        }
		//WIFI_vMainFunc();
		vTaskDelay((*(uint8_t*)pvParameters) / portTICK_PERIOD_MS);
	}
    vTaskDelete(NULL);
}

/* Event handler for catching Wifi events */
static void WIFI_vEventHandler(void* arg, esp_event_base_t event_base, int event_id, void* event_data)
{
	static uint8_t u8RetryCounter = 0;
	tcpip_adapter_ip_info_t ip_info;

	esp_err_t err;

	if ((event_base == WIFI_EVENT) || (event_base == IP_EVENT))
	{  
		switch(event_id) 
		{
			case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG,"Wifi Started, connect to saved AccessPoint");
				err = esp_wifi_connect();
                (void)err;
			break;

			case IP_EVENT_STA_GOT_IP:
                u8RetryCounter = 0;
				ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
				memcpy(WIFI_acIPAddress,&ip_info.ip,sizeof(WIFI_acIPAddress));
				// print the local IP address
				ESP_LOGI(TAG,"IP Address:  %s", ip4addr_ntoa(&ip_info.ip));
				ESP_LOGI(TAG,"Subnet mask: %s", ip4addr_ntoa(&ip_info.netmask));
				ESP_LOGI(TAG,"Gateway:     %s", ip4addr_ntoa(&ip_info.gw));
				/* Signal to continue execution */
				xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
				xEventGroupClearBits(wifi_event_group, CONNECTION_FAILED_BIT);
			break;

			case WIFI_EVENT_STA_DISCONNECTED:

                ESP_LOGI(TAG,"Station Disconnect from AP");
                if(u8RetryCounter < 5)
                {
                    esp_wifi_connect();
                    u8RetryCounter++;
                    ESP_LOGI(TAG, "Retry to connect to the AP");
                } else {
                    ESP_LOGI(TAG, "Failed to connect to the AP");
                    u8RetryCounter = 0;
                    memset(WIFI_acIPAddress,0,sizeof(WIFI_acIPAddress));
                    xEventGroupSetBits(wifi_event_group, CONNECTION_FAILED_BIT);
                    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
                }
			break;

            case WIFI_EVENT_AP_STACONNECTED:
                //wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
                //ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
            break;

            case WIFI_EVENT_AP_STADISCONNECTED:
                //wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
                //ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
            break;

			default:
			break;
		}
	}
	//return ESP_OK;
}

static void WIFI_vConfigSoftAP(void)
{
    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = "ESP32_SoftAP",
            .ssid_len = sizeof("ESP32_SoftAP"),
            .channel = 0,
            .password = "pass1234",
            .max_connection = 1,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    ESP_LOGI(__FUNCTION__,"Wifi AccessPoint SSID: %s with password: %s", wifi_ap_config.ap.ssid, wifi_ap_config.ap.password);
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_ap_config));
}

static void WIFI_vConfigStation(void)
{
    esp_err_t err = ESP_OK;
	uint8_t u8Length = 0;
	uint8_t u8TempBuffer[33] = {};
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	        //.threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    //read from NVS the Wifi SSID and Password string and their length
	err = storage_errReadWifiSSID((char*)u8TempBuffer, &u8Length);
	memcpy(wifi_config.sta.ssid, u8TempBuffer, u8Length);
	#if 0
	printf("%s%s%s%d \n", "Wifi : ", u8TempBuffer, " loaded: ", err);
	printf("Connecting to %s ", wifi_config.sta.ssid);
	printf("ssid length %d \n", u8Length); 
    #endif

	memset(u8TempBuffer,0,sizeof(u8TempBuffer));
	u8Length = 0;	
	err = storage_errReadWifiPass((char*)u8TempBuffer, &u8Length);
	memcpy(wifi_config.sta.password, u8TempBuffer, u8Length);

    #if 0
	printf("%s%s%s%d \n", "Wifi PASS: ", u8TempBuffer, " loaded: ", err);
	printf(" using password %s\n", wifi_config.sta.password);
	printf("pass length %d", u8Length);
    #endif
    (void)err;
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
}

static tenWifiModes WIFI_enCheckProvisioningData(void)
{
    uint8_t u8ProvAvlFlag = 0;
    tenWifiModes enWifiMode = enNrOfModes;

    esp_err_t ret = storage_errReadProvisioningAvailableFlag(&u8ProvAvlFlag);
    //printf("prov flag: %d\n", u8ProvAvlFlag);

    if((ret == ESP_OK) && (u8ProvAvlFlag != 0x00))
    {
        enWifiMode = enStationMode;
    }
    else
    {
        enWifiMode = enSoftAPMode;
    }
    //printf("mode: %d\n", enWifiMode);
    return enWifiMode;
}

//EOF
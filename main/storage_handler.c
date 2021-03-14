#include <stdio.h>
#include "freertos/FreeRTOS.h"
/* System header files*/
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
/* own app header files*/
#include "config.h"
#include "storage_handler.h"

/*************************************************
 * Global Data Definition
 *************************************************/

/*************************************************
 * File Local Data Definition
 ************************************************/
#define STORAGE_NAMESPACE	      	"user_data"
#define STORAGE_NAMESPACE_FCTRY		"fctry_data"
#define STORAGE_KEY_WIFI_SSID		"wifi_ssid"
#define STORAGE_KEY_WIFI_PASS		"wifi_pass"
#define STORAGE_KEY_WIFI_SSID_LEN	"wifi_ssid_len"
#define STORAGE_KEY_WIFI_PASS_LEN	"wifi_pass_len"
#define STORAGE_KEY_SERIAL_NR		"serial_nr"
#define STORAGE_KEY_PROV_AVAILABLE  "prov_available"

#define TAG __FUNCTION__//"NVS_FACTORY_DATA"
//#define TAG __FUNCTION__//"NVS_USER_DATA"
/*************************************************
 * File Local Function Definition and Prototipes
 ************************************************/

/*************************************************
 * External Function Implementation
 ************************************************/
esp_err_t storage_errInitUserNvs(void)
{
    // Set the default wifi logging
	//esp_log_level_set("storage", ESP_LOG_ERROR);
    //esp_log_level_set("storage", ESP_LOG_WARN);
    //esp_log_level_set("storage", ESP_LOG_INFO);
    esp_log_level_set("storage", ESP_LOG_VERBOSE);

    // Initialize USER NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        ESP_LOGI(TAG, "User NVS erase");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
	return err;
}

esp_err_t storage_errDeinit(void)
{
    // De-Initialize NVS
	esp_err_t err;
	err = nvs_flash_deinit();
	return err;
}

esp_err_t storage_errWriteWifiSSID(char *pcStrData, uint8_t u8Length)
{
	esp_err_t err = ESP_OK;
	nvs_handle_t my_handle;

	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
	else 
	{	
		err = nvs_set_u8(my_handle, STORAGE_KEY_WIFI_SSID_LEN, u8Length);
		if(err == ESP_OK)
		{
			err = nvs_set_str(my_handle, STORAGE_KEY_WIFI_SSID, pcStrData);	
		}
		if(err != ESP_OK)
		{
			ESP_LOGE(TAG, "NVS Write Failed!");
		}
		if(err == ESP_OK)
		{
			//Commit written value to ensure it's updated in NVS
			ESP_LOGV(TAG, "Committing updates in NVS ... ");
			err = nvs_commit(my_handle);
			if(err != ESP_OK)
			{
				ESP_LOGE(TAG, "NVS Commit Failed!");
			}
		}
		nvs_close(my_handle); //Close
	}	
	ESP_LOGV(TAG, "Write SSID string: %s with length: %d ",pcStrData, u8Length);
	return err;
}

esp_err_t storage_errWriteWifiPass(char *pcStrData, uint8_t u8Length)
{
	esp_err_t err = ESP_OK;
	nvs_handle_t my_handle;

	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
	else 
	{	
		err = nvs_set_u8(my_handle, STORAGE_KEY_WIFI_PASS_LEN, u8Length);
		if(err == ESP_OK)
		{
			err = nvs_set_str(my_handle, STORAGE_KEY_WIFI_PASS, pcStrData);	
		}
		if(err != ESP_OK)
		{
			ESP_LOGE(TAG, "NVS Write Failed!");
		}
		if(err == ESP_OK)
		{
			//Commit written value to ensure it's updated in NVS
			ESP_LOGV(TAG, "Committing updates in NVS ... ");
			err = nvs_commit(my_handle);
			if(err != ESP_OK)
			{
				ESP_LOGE(TAG, "NVS Commit Failed!");
			}
		}
		nvs_close(my_handle); //Close
	}	
	ESP_LOGV(TAG, "Write password string: %s with length: %d ",pcStrData, u8Length);
	return err;
}

esp_err_t storage_errReadWifiSSID(char *pcStrData, uint8_t *u8Length)
{
	esp_err_t err = ESP_OK;
	nvs_handle_t my_handle;
	
	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }	
	else 
	{
		err = nvs_get_u8(my_handle, STORAGE_KEY_WIFI_SSID_LEN, u8Length);
        switch (err) 
		{
            case ESP_OK:
                ESP_LOGV(TAG, "Done");
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!");
                break;
            default :
                ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
        }
		if(err == ESP_OK)
		{
			err = nvs_get_str(my_handle, STORAGE_KEY_WIFI_SSID, pcStrData, (size_t*)u8Length);
			switch (err) 
			{
				case ESP_OK:
					ESP_LOGV(TAG, "Done");
					break;
				case ESP_ERR_NVS_NOT_FOUND:
					ESP_LOGE(TAG, "The value is not initialized yet!");
					break;
				default :
					ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
			}
		}
		nvs_close(my_handle); //Close
	}
	ESP_LOGV(TAG, "Read SSID string: %s with length: %d ",pcStrData, *u8Length);
	
	return err;
}

esp_err_t storage_errReadWifiPass(char *pcStrData, uint8_t *u8Length)
{
	esp_err_t err = ESP_OK;
	nvs_handle_t my_handle;
	
	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }	
	else 
	{
		err = nvs_get_u8(my_handle, STORAGE_KEY_WIFI_PASS_LEN, u8Length);
        switch (err) 
		{
			case ESP_OK:
					ESP_LOGV(TAG, "Done");
					break;
			case ESP_ERR_NVS_NOT_FOUND:
					ESP_LOGE(TAG, "The value is not initialized yet!");
					break;
			default :
					ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
		}
		if(err == ESP_OK)
		{
			err = nvs_get_str(my_handle, STORAGE_KEY_WIFI_PASS, pcStrData, (size_t*)u8Length);
			switch (err) 
			{
				case ESP_OK:
					ESP_LOGV(TAG, "Done");
					break;
				case ESP_ERR_NVS_NOT_FOUND:
					ESP_LOGE(TAG, "The value is not initialized yet!");
					break;
				default :
					ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
			}
		}
		nvs_close(my_handle); //Close
	}
	ESP_LOGV(TAG, "Read PASS string: %s with length: %d ",pcStrData, *u8Length);

	return err;	
}

esp_err_t storage_errReadProvisioningAvailableFlag(uint8_t *u8Flag)
{
	esp_err_t err = ESP_OK;
	nvs_handle_t my_handle;
	
	err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        //ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }	
	else 
	{
		err = nvs_get_u8(my_handle, STORAGE_KEY_PROV_AVAILABLE, u8Flag);
        switch (err) 
		{
            case ESP_OK:
                ESP_LOGV(TAG, "Done reading: %d", *u8Flag);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!");
                //ESP_LOGE(TAG, "The value is not initialized yet!");
                break;
            default :
                ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
                //ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
        }
		nvs_close(my_handle); //Close
	}
	return err;
}

esp_err_t storage_errWriteProvisioningAvailableFlag(uint8_t u8Flag)
{
	esp_err_t err = ESP_OK;
	nvs_handle_t my_handle;

	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        //ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
	else 
	{	
		err = nvs_set_u8(my_handle, STORAGE_KEY_PROV_AVAILABLE, u8Flag);
		if(err != ESP_OK)
		{
			ESP_LOGE(TAG, "NVS Write Failed!");
		}
		if(err == ESP_OK)
		{
			//Commit written value to ensure it's updated in NVS
			err = nvs_commit(my_handle);
			if(err != ESP_OK)
			{
				ESP_LOGE(TAG, "NVS Commit Failed!");
			}
		}
		nvs_close(my_handle); //Close
	}	
	return err;
}

//EOF
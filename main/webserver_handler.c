//webserver_handler.c

/* System header files*/
#include "esp_system.h"
#include "esp_spiffs.h"
#include "esp_log.h"

#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "cJSON.h"
#include "esp_err.h"

/* own app header files*/
#include "auxiliary.h"
#include "storage_handler.h"
#include "webserver_handler.h"
#include "wifi_handler.h"

/*************************************************
 * Global Data Definition
 *************************************************/

/*************************************************
 * File Local Data Definition
 ************************************************/
#define TAG __FUNCTION__
//static char TAG[50] = "Http Server - ";

/*************************************************
 * File Local Function Definition and Prototipes
 ************************************************/

/*************************************************
 * Function Implementation (External and Local)
 ************************************************/
static void spiffs_serve(char* resource, struct netconn *conn) // serve static content from SPIFFS
{
	// check if it exists on SPIFFS
	char full_path[100];
	sprintf(full_path, "/spiffs%s", resource);
	printf("+ Serving static resource: %s\n", full_path);
	struct stat st;
	if (stat(full_path, &st) == 0) {
		netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);

		// open the file for reading
		FILE* f = fopen(full_path, "r");
		if(f == NULL) {
			printf("Unable to open the file %s\n", full_path);
			return;
		}

		// send the file content to the client
		char buffer[1024];
		while(fgets(buffer, 1024, f))
        {
			netconn_write(conn, buffer, strlen(buffer), NETCONN_NOCOPY);
		}
		fclose(f);
		fflush(stdout);
	}
	else
    {
		netconn_write(conn, http_404_hdr, sizeof(http_404_hdr) - 1, NETCONN_NOCOPY);
	}
}

static void http_server_netconn_serve(struct netconn *conn)
{
	struct netbuf *inbuf;
	char *buf;
	u16_t buflen;
	esp_err_t err;
    uint8_t u8Length = 0;
    char wifiSSID[32];
    char wifiPass[32];
	err = netconn_recv(conn, &inbuf);

	if (err == ERR_OK)
    {
		// get the request and terminate the string
		netbuf_data(inbuf, (void**)&buf, &buflen);
		buf[buflen] = '\0';
		
		// get the request body and the first line
		char* body = strstr(buf, "\r\n\r\n");
		char *request_line = strtok(buf, "\n");
		
		if(request_line) 
        {
			// dynamic page: setConfig
			if(strstr(request_line, "POST /setConfig"))
            {
                ESP_LOGI(TAG, "Received POST /setConfig");
				cJSON *root = cJSON_Parse(body);

				cJSON *json_item = cJSON_GetObjectItemCaseSensitive(root, "ssid");
                strcpy(wifiSSID, json_item->valuestring); //save SSID received on webinterface
                u8Length = AUX_u8CalcStringLength(wifiSSID, sizeof(wifiSSID));
                err = storage_errWriteWifiSSID(wifiSSID, u8Length);  //save to non volatile storage

                json_item = cJSON_GetObjectItemCaseSensitive(root, "pass");
                strcpy(wifiPass, json_item->valuestring); //save password received on webinterface
                u8Length = AUX_u8CalcStringLength(wifiPass, sizeof(wifiPass));
                err |= storage_errWriteWifiPass(wifiPass, u8Length);
                
                if(err == ESP_OK)
                {
                    ESP_LOGI(TAG, "Received SSID: %s and Password: %s saved successefully to NVM", wifiSSID, wifiPass);
                }
                else
                {
                    ESP_LOGE(TAG, "Could not save SSID and Password in NVM, error: %s", esp_err_to_name(err));
                }

                //set provisioning available Flag in NVM, so that if the WifiTask in AP Mode in it's next cycle can configure the StationMode

                err = storage_errWriteProvisioningAvailableFlag(0x01);
                if(err == ESP_OK)
                {
                    ESP_LOGI(TAG, "Provisioning available flag saved successefully to NVM");
                }
                else
                {
                    ESP_LOGE(TAG, "Could not save Provisioning available flag in NVM, error: %s", esp_err_to_name(err));
                }

			}
            /*
			else if(strstr(request_line, "GET /getConfig")) // dynamic page: getConfig
            {
                ESP_LOGI(TAG, "Received GET /getConfig");
				cJSON *root = cJSON_CreateObject();
                cJSON_AddStringToObject(root, "dev_info", pcMessageForUser);
                cJSON_AddNumberToObject(root, "x_pos", i32XPosAvg);					
				char *rendered = cJSON_Print(root);
				netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);
				netconn_write(conn, rendered, strlen(rendered), NETCONN_NOCOPY);
			}
            */
			else if(strstr(request_line, "GET / ")) // default page -> redirect to index.html
            {
                ESP_LOGI(TAG, "Received GET /");
				spiffs_serve("/index.html", conn);
			}
			else // static content, get it from SPIFFS
            {
				// get the requested resource
				char* method = strtok(request_line, " ");
				char* resource = strtok(NULL, " ");
                (void)method;
				spiffs_serve(resource, conn);
			}
		}
	}
	
	// close the connection and free the buffer
	netconn_close(conn);
	netbuf_delete(inbuf);
}

void vTaskHttpServer(void *pvParameters)
{
	struct netconn *conn, *newconn;
	err_t err;
    ESP_LOGI(TAG,"Starting Http Webserver Task with %d ms period!", (*(uint8_t*)pvParameters));

	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, NULL, 80);
	netconn_listen(conn);
	ESP_LOGI(TAG,"* HTTP Server listening");
	do {
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK) 
        {
			http_server_netconn_serve(newconn);
			netconn_delete(newconn);
		}
		vTaskDelay((*(uint8_t*)pvParameters)); //allows task to be pre-empted
	} while(err == ERR_OK);
	netconn_close(conn);
	netconn_delete(conn);
}
//EOF
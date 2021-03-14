//config.h
#ifndef CFG_H
#define CFG_H

/* Put here configurations that impact the behaviour of the product (eg. task delays, pin assignments, message format, etc)
 * and definitions that are used in more modules.
 * DO NOT put here symbol definitions, macros, and internal module configurations (eg buffer size, )
 */
#define FIRMWARE_VERSION 0.022      //tracker firmware version

#define BR_MSG_TIMECODE_BYTE_POS            (0)
#define BR_MSG_DWM_XCOORD_BYTE_POS          (1)
#define BR_MSG_DWM_YCOORD_BYTE_POS          (5)
#define BR_MSG_DWM_ZCOORD_BYTE_POS          (9)
#define BR_MSG_DWM_SQF_BYTE_POS             (13)
#define BR_MSG_ENC_FOCUS_ROTATION_BYTE_POS  (14)
#define BR_MSG_ENC_FOCUS_POSITION_BYTE_POS  (15)
#define BR_MSG_ENC_ZOOM_ROTATION_BYTE_POS   (16)
#define BR_MSG_ENC_ZOOM_POSITION_BYTE_POS   (17)
#define BR_MSG_ENC_IRIS_ROTATION_BYTE_POS   (18)
#define BR_MSG_ENC_IRIS_POSITION_BYTE_POS   (19)
#define BR_MSG_COOKE_LENS_DATA_BYTE_POS     (20)

#define BR_MSG_DWM_ERROR_BYTE_POS           (0)
#define BR_MSG_ENC_FOCUS_ERROR_BYTE_POS     (1)
#define BR_MSG_ENC_ZOOM_ERROR_BYTE_POS      (2)
#define BR_MSG_ENC_IRIS_ERROR_BYTE_POS      (3)
#define BR_MSG_COOKE_LENS_ERROR_BYTE_POS    (4)

/*************************************************
 * WiFi related configurations
 ************************************************/
#define WIFI_nRetryConnectionTime	        (10) //units of WIFI_WifiTaskDelayMs, ex. 10 = 10 seconds
#define WIFI_WifiTaskDelayMs	            (1000) //delay of WifiHandler Task, ex. 1000 = 1 second
#define WIFI_nMaxRetryBeforNewProvisioning  (3) //nr of retries with old credentials before restarting provisioning

#define WIFI_AP_SSID            "AnchorAP_"
#define WIFI_AP_PASS            "anchorpass"

#define WIFI_PROV_USE_SEC_LVL_1 (0) //set to 1 to use security level 1 for provisioning, 0 otherwise
#define WIFI_PROV_USE_POP       (0) //set to 1 to use proof of possesion for provisioning, 0 otherwise
#define WIFI_PROV_USE_STATIC_AP_SSID    (0) //set to 1 to use predefined SSID for SoftAP, 0 otherwise
#define WIFI_PROV_TEMP_POP      "temp"

#define WIFI_MAX_CREDENTIALS_LENGTH (33)

/*************************************************
 * UDP Client related configurations
 ************************************************/
#define UDP_CLI_MaxSendRetry    (50)

/*************************************************
 * Status Led and GPIO related configurations
 ************************************************/
#define GPIO_StatusLedTaskDelayMs (1000)
#define GPIO_RGB_RED_PIN    (GPIO_NUM_25)
#define GPIO_RGB_GREEN_PIN  (GPIO_NUM_27)
#define GPIO_RGB_BLUE_PIN   (GPIO_NUM_26)
#define GPIO_RGB_OUTPUT_PIN_SEL ((1 << GPIO_RGB_RED_PIN) | (1 << GPIO_RGB_GREEN_PIN) | (1 << GPIO_RGB_BLUE_PIN))

/*************************************************
 * DWM related configurations
 ************************************************/
#define DWM_NrOfSamplesInAvg    (10)

#endif  //CFG_H
//EOF
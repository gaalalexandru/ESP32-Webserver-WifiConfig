# Hello World

It is often necessary, to not have the Wifi credentials hardcoded in the source code of your software (eg: #define WIFI_SSID "MyHardCodedSSID"),
rather let the user set up the SSID and password when it starts up the device.

This example software will provide a method to configure the ESP32 WiFi Station through a webinterface.

When the ESP32 Station can't connect to an AccesPoint, it will start SoftAP mode and a webserver.
The webserver will publish us a HTML webpage that is stored in a SPIFFS partition.

The user can connect to the ESP32 SoftAP, in this example "ESP32_SoftAP" with password: "pass1234".
In the browser 192.168.4.2 has to be accessed to see the webinterface.

The user can provide new wifi credentials and the ESP32 will try to connect to that AccessPoint.
If succedes, it will shut down the SoftAP mode.
If not, it will retry a few times and re-start SoftAP mode. 


The HTML 
To manually generate the SPIFFS image run: "python spiffsgen.py 0xF0000 spiffs_content spiffs_image/out.img"
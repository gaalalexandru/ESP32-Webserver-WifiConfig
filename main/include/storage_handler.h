#ifndef STORAGE_H
#define STORAGE_H

esp_err_t storage_errInitUserNvs(void);
esp_err_t storage_errDeinit(void);
esp_err_t storage_errWriteWifiSSID(char *pcStrData, uint8_t u8Length);
esp_err_t storage_errWriteWifiPass(char *pcStrData, uint8_t u8Length);
esp_err_t storage_errReadWifiSSID(char *pcStrData, uint8_t *u8Length);
esp_err_t storage_errReadWifiPass(char *pcStrData, uint8_t *u8Length);
esp_err_t storage_errReadProvisioningAvailableFlag(uint8_t *u8Flag);
esp_err_t storage_errWriteProvisioningAvailableFlag(uint8_t u8Flag);

#endif //STORAGE_H

//EOF
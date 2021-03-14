#ifndef AUX_H
#define AUX_H

//#define AUX_BROADCAST_ERROR_RAISE(x)	TKBLE_au8PeripherialHwBroadcastError |= (1<< (x));
//#define AUX_BROADCAST_ERROR_CLEAR(x)	TKBLE_au8PeripherialHwBroadcastError &~ (1<< (x));

void AUX_print_addr(const void *addr);
void AUX_print_bytes(const uint8_t *bytes, int len);
uint8_t AUX_u8CalcStringLength(const char *pcChar, uint8_t u8Maxlen);
uint8_t AUX_u8ExtractIPAddress(const char *pcChar, char *au8IPAddress);
int32_t AUX_i32CalculateMovingAvg(int32_t *ptrArrNumbers, int32_t *ptrSum, uint8_t pos, uint8_t len, int32_t nextNum);
#endif
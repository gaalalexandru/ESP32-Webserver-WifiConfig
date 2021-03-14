
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "console/console.h"

#include "auxiliary.h"

void AUX_print_addr(const void *addr)
{
    const uint8_t *u8p;

    u8p = addr;
    //MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x", u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
	printf("AUX INFO: %02x:%02x:%02x:%02x:%02x:%02x", u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}


void AUX_print_bytes(const uint8_t *bytes, int len)
{
    int i;
    for (i = 0; i < len; i++)
	{
        //MODLOG_DFLT(INFO, "%s0x%02x", i != 0 ? ":" : "", bytes[i]);
		printf("AUX INFO: %s0x%02x", i != 0 ? ":" : "", bytes[i]);
    }
}

uint8_t AUX_u8CalcStringLength(const char *pcChar, uint8_t u8Maxlen)
{
	uint8_t u8Length = 0;
	
	while((*pcChar++) && (u8Maxlen > u8Length))
	{
		u8Length++;
	}
	u8Length++;  //for the \0 character at the end of the string
	
	return u8Length;
}
/*pcChar[] = "192.168.1.195\0"*/
uint8_t AUX_u8ExtractIPAddress(const char *pcChar, char *ac8IPAddress)
{
    uint8_t u8Index = 0;
    memset(ac8IPAddress, 0, 4);
    while(*pcChar)
    {
        if(*pcChar == '.')
        {
            u8Index++;
        }
        else
        {
            ac8IPAddress[u8Index] = ac8IPAddress[u8Index]*10 + (*pcChar - 0x30);
            //printf("ac8IPAddress[%d] = %d\n", u8Index, ac8IPAddress[u8Index]);
        }
        pcChar++;
    }
    //printf("Calculated IP address: %d.%d.%d.%d\n", ac8IPAddress[0], ac8IPAddress[1], ac8IPAddress[2], ac8IPAddress[3]);
    return (u8Index+1);
}

int32_t AUX_i32CalculateMovingAvg(int32_t *ptrArrNumbers, int32_t *ptrSum, uint8_t pos, uint8_t len, int32_t nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}

//EOF
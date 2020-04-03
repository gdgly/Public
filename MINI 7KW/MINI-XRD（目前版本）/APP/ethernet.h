#ifndef __ETHERNET_H
#define __ETHERNET_H
#include "sys.h"

uint8_t EthConn(uint8_t sn, uint8_t serverip[4], uint16_t serverport);
uint8_t EthSend(uint8_t sn, uint8_t *buffer, uint16_t len);
uint16_t EthRecv(uint8_t sn, uint8_t *buffer, uint16_t len);
uint8_t GetLinkStatus(void);

void EthHandle(void);
uint8_t EthInit(void);
void EthTask(void *pdata);

#endif







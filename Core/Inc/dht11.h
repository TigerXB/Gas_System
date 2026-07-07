#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"

/* DHT11 sensor driver */
void DHT11_Init(void);
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi);

#endif
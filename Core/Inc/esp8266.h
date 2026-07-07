// 文件名：esp8266.h
#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"

/* WiFi and Bemfa Cloud Configuration */
#define WIFI_SSID   "Atomic"
#define WIFI_PWD    "********"

#define BEMFA_UID   "********************************"   /* 替换为你的巴法云UID */
#define BEMFA_TOPIC "GasMonitor001"

/* Public API */
void ESP8266_Init(void);
void ESP8266_SendData(uint32_t mq2, uint32_t mq135, uint8_t temp, uint8_t humi);

#endif /* __ESP8266_H */
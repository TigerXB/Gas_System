#include "esp8266.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart2;

/* UART send wrapper */
static void ESP_SendCmd(char *cmd) 
{
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), 1000);
}

void ESP8266_Init(void) 
{
    char cmd_buf[128]; 

    /* Exit transparent mode to avoid connection issues after MCU reset */
    ESP_SendCmd("+++");
    HAL_Delay(1000);

    /* Set Station mode */
    ESP_SendCmd("AT+CWMODE=1\r\n"); 
    HAL_Delay(500);
    
    /* Connect to WiFi */
    sprintf(cmd_buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PWD);
    ESP_SendCmd(cmd_buf);
    HAL_Delay(8000); 
    
    /* Enable transparent transmission mode */
    ESP_SendCmd("AT+CIPMODE=1\r\n");
    HAL_Delay(500);

    /* Connect to Bemfa Cloud TCP server */
    ESP_SendCmd("AT+CIPSTART=\"TCP\",\"bemfa.com\",8344\r\n");
    HAL_Delay(2000);

    /* Enter send mode — subsequent data goes directly to server */
    ESP_SendCmd("AT+CIPSEND\r\n");
    HAL_Delay(500);

    /* Send subscription message to Bemfa Cloud */
    sprintf(cmd_buf, "cmd=1&uid=%s&topic=%s\r\n", BEMFA_UID, BEMFA_TOPIC);
    ESP_SendCmd(cmd_buf);
    HAL_Delay(500);
}

void ESP8266_SendData(uint32_t mq2, uint32_t mq135, uint8_t temp, uint8_t humi) 
{
    char pub_buf[128]; 
    
    /* Bemfa data publish format: cmd=2 */
    sprintf(pub_buf, "cmd=2&uid=%s&topic=%s&msg=MQ2:%d,MQ135:%d,T:%d,H:%d\r\n", 
            BEMFA_UID, BEMFA_TOPIC, (int)mq2, (int)mq135, (int)temp, (int)humi);
            
    ESP_SendCmd(pub_buf);
}
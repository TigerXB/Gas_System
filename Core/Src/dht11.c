#include "dht11.h"

/* Microsecond delay (SysTick-based) */
void delay_us(uint32_t us) {
    uint32_t delay = us * (HAL_RCC_GetHCLKFreq() / 4000000);
    while (delay--) { __asm("nop"); }
}

void DHT11_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;   /* open-drain */
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void DHT11_Start(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_Delay(25);                                /* pull low 25ms, start signal */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    delay_us(30);  
}

uint8_t DHT11_Check(void) {
    uint16_t retry = 0;                           /* timeout counter */
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET && retry < 10000) {
        retry++; 
    }
    if (retry >= 10000) return 1; else retry = 0;
    
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET && retry < 10000) {
        retry++; 
    }
    if (retry >= 10000) return 1;
    return 0;
}

/* Read a single bit from DHT11 data line */
uint8_t DHT11_Read_Bit(void) {
    uint16_t low_time = 0;
    uint16_t high_time = 0;
    uint16_t retry = 0;
    
    /* Skip low-level preamble (~50us) */
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET && retry < 10000) retry++;
    
    /* Record low-level duration (fixed ~50us from DHT11) */
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET && low_time < 10000) {
        low_time++; 
    }
    
    /* Record high-level duration (bit 0: ~28us, bit 1: ~70us) */
    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET && high_time < 10000) {
        high_time++; 
    }
    
    /* Decision: longer high-level => bit 1, else bit 0.
       Compare against low_time to filter noise. */
    if (high_time > low_time) return 1;
    else return 0;
}

uint8_t DHT11_Read_Byte(void) {
    uint8_t i, dat = 0;
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi) {
    uint8_t buf[5];
    uint8_t i;
    DHT11_Start();
    if (DHT11_Check() == 0) {
        for (i = 0; i < 5; i++) {
            buf[i] = DHT11_Read_Byte();
        }
        /* Verify checksum */
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) {
            *humi = buf[0];
            *temp = buf[2];
            return 0;                             /* success */
        }
    }
    return 1;                                     /* failure */
}
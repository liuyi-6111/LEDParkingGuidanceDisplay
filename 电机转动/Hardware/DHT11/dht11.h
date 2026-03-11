#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"

// DHT11引脚定义 - 使用PB5
#define DHT11_GPIO_PORT     GPIOB
#define DHT11_GPIO_PIN      GPIO_Pin_5
#define DHT11_GPIO_CLK      RCC_APB2Periph_GPIOB

// 电平控制宏
#define DHT11_OUT_LOW()     GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)
#define DHT11_OUT_HIGH()    GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)
#define DHT11_IN_READ()     GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

// 函数声明
void DHT11_Init(void);
uint8_t DHT11_ReadData(uint8_t* temp, uint8_t* humi);

#endif
#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"

// DHT11 引脚定义 - 使用 PB9
#define DHT11_GPIO_PORT     GPIOB
#define DHT11_GPIO_PIN      GPIO_Pin_9
#define DHT11_GPIO_CLK      RCC_APB2Periph_GPIOB

// 宏定义操作
#define DHT11_OUT_H()       GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)
#define DHT11_OUT_L()       GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)
#define DHT11_IN_READ()     GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

// DHT11 数据类型结构体
typedef struct {
    uint8_t humidity_int;      // 湿度整数部分
    uint8_t humidity_deci;     // 湿度小数部分
    uint8_t temperature_int;   // 温度整数部分
    uint8_t temperature_deci;  // 温度小数部分
    uint8_t checksum;          // 校验和
} DHT11_Data_TypeDef;

// 函数声明
void DHT11_Init(void);
uint8_t DHT11_ReadData(DHT11_Data_TypeDef* dht11_data);
void DHT11_Delay_us(uint32_t us);

#endif /* __DHT11_H */
#ifndef __MAX7219_H
#define __MAX7219_H

#include "stm32f10x.h"

// 引脚定义
#define MAX7219_DIN_PORT   GPIOA
#define MAX7219_DIN_PIN    GPIO_Pin_7
#define MAX7219_CS_PORT    GPIOA
#define MAX7219_CS_PIN     GPIO_Pin_4
#define MAX7219_CLK_PORT   GPIOA
#define MAX7219_CLK_PIN    GPIO_Pin_5

// 全局变量声明
extern uint16_t d;
extern uint8_t modeflag;
extern uint8_t display_buffer[8];

// 函数声明
void MAX7219_Init(void);
void MAX7219_WriteByte(uint8_t data);
void MAX7219_Write(uint8_t addr, uint8_t data);
void MAX7219_Clear(void);
void MAX7219_SetRow(uint8_t row, uint8_t data);
void MAX7219_SetBrightness(uint8_t brightness);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void led_delay(uint16_t xus);

#endif

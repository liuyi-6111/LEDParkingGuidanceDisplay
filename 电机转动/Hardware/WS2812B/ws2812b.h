#ifndef __WS2812B_H
#define __WS2812B_H

#include "stm32f10x.h"

// 配置参数
#define WS2812B_NUM_LEDS    20

// 全局变量声明
extern uint8_t led_buffer[WS2812B_NUM_LEDS][3];
extern uint16_t num_leds;

// 函数声明
void WS2812B_Init(void);
void WS2812B_SetLED(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812B_SetAll(uint8_t r, uint8_t g, uint8_t b);
void WS2812B_Clear(void);
void WS2812B_Update(void);
void WS2812B_Rainbow(uint8_t start_hue);

// 延时函数声明（已在其他地方定义）
void delay_ms(uint32_t ms);

#endif
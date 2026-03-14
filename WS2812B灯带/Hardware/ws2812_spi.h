#ifndef __WS2812_SPI_H
#define __WS2812_SPI_H

#include "stm32f10x.h"

// ==================== 配置参数 ====================
#define LED_NUM             6           // 你的灯珠数量是6颗
#define SPI_BUFFER_SIZE     (LED_NUM * 24 * 3)

// 颜色宏定义（顺序：GRB）
#define COLOR_RED           0x00FF00    // 红色
#define COLOR_GREEN         0xFF0000    // 绿色
#define COLOR_BLUE          0x0000FF    // 蓝色
#define COLOR_YELLOW        0xFFFF00    // 黄色
#define COLOR_CYAN          0xFF00FF    // 青色
#define COLOR_MAGENTA       0x00FFFF    // 品红
#define COLOR_WHITE         0xFFFFFF    // 白色
#define COLOR_BLACK         0x000000    // 黑色

// 函数声明
void SPI_WS2812_Init(void);
void WS2812_SetPixelColor(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
void WS2812_SetPixelColor_GRB(uint16_t index, uint32_t grb_color);
void WS2812_SetAllColor(uint8_t red, uint8_t green, uint8_t blue);
void WS2812_Update(void);
void WS2812_Clear(void);
void Delay_us(uint32_t us);

#endif /* __WS2812_SPI_H */

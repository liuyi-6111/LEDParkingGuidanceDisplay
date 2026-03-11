#include "ws2812b.h"  // 注意：这里应该是 ws2812b.h，不是 ws2812b.h

// 定义数据引脚，沿用之前的PA7
#define WS_DATA_PORT   GPIOA
#define WS_DATA_PIN    GPIO_Pin_7

// GRB缓冲区
static uint8_t led_buffer[WS2812B_NUM_LEDS][3];

// 宏定义，快速操作IO口
#define WS_HIGH()    (WS_DATA_PORT->BSRR = WS_DATA_PIN)
#define WS_LOW()     (WS_DATA_PORT->BRR  = WS_DATA_PIN)

// 全局变量定义
uint16_t num_leds = WS2812B_NUM_LEDS;

// 精确延时函数（72MHz主频）
static void delay_ws(uint32_t cycles) {
    volatile uint32_t i;
    for (i = 0; i < cycles; i++) {
        __NOP();
    }
}

// 发送一个bit
static void ws2812b_send_bit(uint8_t bit) {
    if (bit) {
        WS_HIGH();
        delay_ws(14);  // 约0.7-0.8us
        WS_LOW();
        delay_ws(7);   // 约0.35-0.45us
    }
    else {
        WS_HIGH();
        delay_ws(7);   // 约0.35-0.4us
        WS_LOW();
        delay_ws(14);  // 约0.7-0.8us
    }
}

// 发送一个字节 (MSB first)
static void ws2812b_send_byte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        ws2812b_send_bit((byte >> i) & 0x01);
    }
}

// 初始化IO口
void WS2812B_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = WS_DATA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(WS_DATA_PORT, &GPIO_InitStructure);

    WS2812B_Clear();
    WS2812B_Update();
}

// 设置某个LED的颜色
void WS2812B_SetLED(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < WS2812B_NUM_LEDS) {
        led_buffer[index][0] = g; // GRB顺序
        led_buffer[index][1] = r;
        led_buffer[index][2] = b;
    }
}

// 设置所有LED颜色
void WS2812B_SetAll(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        WS2812B_SetLED(i, r, g, b);
    }
}

// 清屏
void WS2812B_Clear(void) {
    WS2812B_SetAll(0, 0, 0);
}

// 更新显示
void WS2812B_Update(void) {
    __disable_irq();

    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        ws2812b_send_byte(led_buffer[i][0]); // G
        ws2812b_send_byte(led_buffer[i][1]); // R
        ws2812b_send_byte(led_buffer[i][2]); // B
    }

    // 复位信号
    WS_LOW();
    delay_ws(1000);  // 约50us

    __enable_irq();
}

// 彩虹效果
void WS2812B_Rainbow(uint8_t start_hue) {
    for (int i = 0; i < WS2812B_NUM_LEDS; i++) {
        uint8_t hue = (start_hue + i * 10) % 256;
        uint8_t r, g, b;

        // 简化的HSV到RGB转换
        if (hue < 85) {
            r = hue * 3;
            g = 255 - hue * 3;
            b = 0;
        }
        else if (hue < 170) {
            hue -= 85;
            r = 255 - hue * 3;
            g = 0;
            b = hue * 3;
        }
        else {
            hue -= 170;
            r = 0;
            g = hue * 3;
            b = 255 - hue * 3;
        }

        WS2812B_SetLED(i, r, g, b);
    }
}
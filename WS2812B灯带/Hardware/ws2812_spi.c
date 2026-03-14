#include "ws2812_spi.h"
#include <string.h>

// 重要：经过实测，以下参数能让6颗WS2812在3.3V/5V下全亮
// BIT_0=0xE0, BIT_1=0xFC, SPI速度=4.5MHz
// 实测有效
// ==================== 配置参数 ====================
#define WS2812_SPI_BIT_0  0xE0    // 1110 0000
#define WS2812_SPI_BIT_1  0xFC    // 1111 1100
#define WS2812_RESET_US   60      // 复位信号长度

// ==================== 全局变量 ====================
static uint8_t led_spi_buffer[LED_NUM][24 * 3];
static uint8_t led_color_buffer[LED_NUM][3];

// ==================== 微秒级延时函数 ====================
void Delay_us(uint32_t us)
{
    uint32_t i;
    for (i = 0; i < us; i++)
    {
        uint32_t cycles = 72;
        while (cycles--)
        {
            __NOP();
        }
    }
}

// ==================== SPI初始化 ====================
void SPI_WS2812_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 1. 使能时钟 - 【修改点】GPIOA改成GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOB, ENABLE);

    // 2. 配置PB5 (SPI1_MOSI) 为复用推挽输出 - 【修改点】PA7改成PB5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. SPI1配置（这部分完全不变）
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;  // 4.5MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 4. 使能SPI
    SPI_Cmd(SPI1, ENABLE);

    // 5. 初始化颜色缓冲区为0
    WS2812_Clear();
}

// ==================== 颜色编码函数 ====================
static void WS2812_EncodeColor(uint16_t index, uint32_t grb_color)
{
    uint8_t* p_buffer = led_spi_buffer[index];

    for (int i = 23; i >= 0; i--)
    {
        if (grb_color & (1 << i))
        {
            *p_buffer++ = WS2812_SPI_BIT_1;
        }
        else
        {
            *p_buffer++ = WS2812_SPI_BIT_0;
        }
    }
}

// ==================== 设置单个LED颜色 ====================
void WS2812_SetPixelColor(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index >= LED_NUM) return;

    led_color_buffer[index][0] = green;
    led_color_buffer[index][1] = red;
    led_color_buffer[index][2] = blue;

    uint32_t grb_color = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
    WS2812_EncodeColor(index, grb_color);
}

// ==================== 设置所有LED为相同颜色 ====================
void WS2812_SetAllColor(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t grb_color = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;

    for (uint16_t i = 0; i < LED_NUM; i++)
    {
        led_color_buffer[i][0] = green;
        led_color_buffer[i][1] = red;
        led_color_buffer[i][2] = blue;
        WS2812_EncodeColor(i, grb_color);
    }
}

// ==================== 关闭所有LED ====================
void WS2812_Clear(void)
{
    for (uint16_t i = 0; i < LED_NUM; i++)
    {
        led_color_buffer[i][0] = 0;
        led_color_buffer[i][1] = 0;
        led_color_buffer[i][2] = 0;

        uint8_t* p_buffer = led_spi_buffer[i];
        for (int j = 0; j < 24 * 3; j++)
        {
            *p_buffer++ = WS2812_SPI_BIT_0;
        }
    }
}

// ==================== 更新显示 ====================
void WS2812_Update(void)
{
    // 发送所有LED的数据（这部分完全不变）
    for (uint16_t led_idx = 0; led_idx < LED_NUM; led_idx++)
    {
        for (uint16_t byte_idx = 0; byte_idx < 24 * 3; byte_idx++)
        {
            while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
            SPI_I2S_SendData(SPI1, led_spi_buffer[led_idx][byte_idx]);
        }
    }

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

    // ===== 发送复位信号 - 【修改点】所有PA7改成PB5 =====
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 把PB5配置为普通推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 拉低PB5，发送复位信号
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    Delay_us(WS2812_RESET_US);

    // 恢复PB5为复用推挽输出（SPI模式）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 重新使能SPI
    SPI_Cmd(SPI1, ENABLE);
}

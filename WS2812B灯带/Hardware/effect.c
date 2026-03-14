#include "ws2812_spi.h"
#include <string.h>

// ==================== 基础流水灯效果 ====================
void LED_Effect_BasicFlow(void)
{
    // 单灯流水（从左到右）- 红色
    for (int i = 0; i < LED_NUM; i++)
    {
        WS2812_Clear();
        WS2812_SetPixelColor(i, 50, 0, 0);  // (R,G,B) = 红
        WS2812_Update();
        Delay_ms(150);
    }

    // 单灯流水（从右到左）- 绿色
    for (int i = LED_NUM - 1; i >= 0; i--)
    {
        WS2812_Clear();
        WS2812_SetPixelColor(i, 0, 50, 0);  // (R,G,B) = 绿
        WS2812_Update();
        Delay_ms(150);
    }
    
    // 单灯流水（从左到右）- 蓝色
    for (int i = 0; i < LED_NUM; i++)
    {
        WS2812_Clear();
        WS2812_SetPixelColor(i, 0, 0, 50);  // (R,G,B) = 蓝
        WS2812_Update();
        Delay_ms(150);
    }
}

// ==================== 双灯流水效果 ====================
void LED_Effect_DoubleFlow(void)
{
    // 两个灯一起流
    for (int i = 0; i < LED_NUM + 1; i++)
    {
        WS2812_Clear();

        if (i < LED_NUM) WS2812_SetPixelColor(i, 50, 0, 0);      // 红
        if (i - 1 >= 0) WS2812_SetPixelColor(i - 1, 0, 50, 0);  // 绿

        WS2812_Update();
        Delay_ms(200);
    }
    
    // 反向流（蓝+紫）
    for (int i = LED_NUM; i >= -1; i--)
    {
        WS2812_Clear();
        
        if (i < LED_NUM && i >= 0) WS2812_SetPixelColor(i, 0, 0, 50);      // 蓝
        if (i+1 < LED_NUM && i+1 >= 0) WS2812_SetPixelColor(i+1, 50, 0, 50); // 紫
        
        WS2812_Update();
        Delay_ms(200);
    }
}

// ==================== 三色流水效果 ====================
void LED_Effect_ThreeColorFlow(void)
{
    for (int i = 0; i < LED_NUM + 3; i++)
    {
        WS2812_Clear();

        for (int j = 0; j < 3; j++)
        {
            int idx = i - j;
            if (idx >= 0 && idx < LED_NUM)
            {
                if (j == 0) WS2812_SetPixelColor(idx, 50, 0, 0);    // 红
                if (j == 1) WS2812_SetPixelColor(idx, 0, 50, 0);    // 绿
                if (j == 2) WS2812_SetPixelColor(idx, 0, 0, 50);    // 蓝
            }
        }

        WS2812_Update();
        Delay_ms(200);
    }
}

// ==================== 彩虹流水效果 ====================
void LED_Effect_RainbowFlow(void)
{
    uint8_t colors[7][3] = {
        {50, 0, 0},     // 红 (R,G,B)
        {50, 25, 0},    // 橙
        {50, 50, 0},    // 黄
        {0, 50, 0},     // 绿
        {0, 0, 50},     // 蓝
        {50, 0, 50},    // 紫
        {0, 50, 50}     // 青
    };

    for (int offset = 0; offset < LED_NUM + 7; offset++)
    {
        WS2812_Clear();

        for (int i = 0; i < LED_NUM; i++)
        {
            int color_idx = (i + offset) % 7;
            WS2812_SetPixelColor(i, 
                colors[color_idx][0],  // R
                colors[color_idx][1],  // G
                colors[color_idx][2]); // B
        }

        WS2812_Update();
        Delay_ms(200);
    }
}

// ==================== 呼吸流水效果 ====================
void LED_Effect_BreathingFlow(void)
{
    // 红色呼吸流水
    for (int brightness = 10; brightness <= 50; brightness += 5)
    {
        for (int i = 0; i < LED_NUM; i++)
        {
            WS2812_Clear();
            WS2812_SetPixelColor(i, brightness, 0, 0);  // 红
            WS2812_Update();
            Delay_ms(100);
        }
    }

    // 绿色呼吸流水
    for (int brightness = 50; brightness >= 10; brightness -= 5)
    {
        for (int i = LED_NUM - 1; i >= 0; i--)
        {
            WS2812_Clear();
            WS2812_SetPixelColor(i, 0, brightness, 0);  // 绿
            WS2812_Update();
            Delay_ms(100);
        }
    }
    
    // 蓝色呼吸流水
    for (int brightness = 10; brightness <= 50; brightness += 5)
    {
        for (int i = 0; i < LED_NUM; i++)
        {
            WS2812_Clear();
            WS2812_SetPixelColor(i, 0, 0, brightness);  // 蓝
            WS2812_Update();
            Delay_ms(100);
        }
    }
}

// ==================== 两端对流水效果 ====================
void LED_Effect_BothEndsFlow(void)
{
    // 从两端向中间（红左蓝右）
    for (int i = 0; i < LED_NUM / 2 + 1; i++)
    {
        WS2812_Clear();

        int left = i;
        int right = LED_NUM - 1 - i;

        if (left <= right)
        {
            WS2812_SetPixelColor(left, 50, 0, 0);   // 红
            if (left != right)
                WS2812_SetPixelColor(right, 0, 0, 50); // 蓝
        }

        WS2812_Update();
        Delay_ms(300);
    }

    // 从中间向两端（绿）
    for (int i = LED_NUM / 2; i >= 0; i--)
    {
        WS2812_Clear();

        int left = i;
        int right = LED_NUM - 1 - i;

        if (left <= right)
        {
            WS2812_SetPixelColor(left, 0, 50, 0);   // 绿
            if (left != right)
                WS2812_SetPixelColor(right, 0, 50, 0); // 绿
        }

        WS2812_Update();
        Delay_ms(300);
    }
}

// ==================== 跳跃流水效果 ====================
void LED_Effect_JumpFlow(void)
{
    // 紫色跳跃
    for (int start = 0; start < 2; start++)
    {
        for (int i = start; i < LED_NUM; i += 2)
        {
            WS2812_Clear();
            WS2812_SetPixelColor(i, 50, 0, 50);  // 紫 (R,G,B)
            WS2812_Update();
            Delay_ms(300);
        }
    }
    
    // 青色跳跃
    for (int start = 1; start >= 0; start--)
    {
        for (int i = start; i < LED_NUM; i += 2)
        {
            WS2812_Clear();
            WS2812_SetPixelColor(i, 0, 50, 50);  // 青 (R,G,B)
            WS2812_Update();
            Delay_ms(300);
        }
    }
}

#include "stm32f10x.h"
#include "ws2812_spi.h"

void Delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++)
        for(uint32_t j = 0; j < 8000; j++) __NOP();
}

int main(void)
{
    SPI_WS2812_Init();
    
    while(1)
    {
        // ===== 测试1：红色 =====
        WS2812_SetAllColor(50, 0, 0);  // 红
        WS2812_Update();
        Delay_ms(2000);
        
        // ===== 测试2：绿色 =====
        WS2812_SetAllColor(0, 50, 0);  // 绿
        WS2812_Update();
        Delay_ms(2000);
        
        // ===== 测试3：蓝色 =====
        WS2812_SetAllColor(0, 0, 50);  // 蓝
        WS2812_Update();
        Delay_ms(2000);
        
        // ===== 测试4：黄色 =====
        WS2812_SetAllColor(50, 50, 0);  // 黄
        WS2812_Update();
        Delay_ms(2000);
        
        // ===== 测试5：紫色 =====
        WS2812_SetAllColor(50, 0, 50);  // 紫
        WS2812_Update();
        Delay_ms(2000);
        
        // ===== 测试6：青色 =====
        WS2812_SetAllColor(0, 50, 50);  // 青
        WS2812_Update();
        Delay_ms(2000);
    }
}

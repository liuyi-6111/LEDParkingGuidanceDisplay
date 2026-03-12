#include "delay.h"

static uint32_t fac_us = 0;  // us 延时倍乘数

/**
 * @brief  延时初始化
 * @param  None
 * @retval None
 */
void Delay_Init(void)
{
    // 配置 SysTick 时钟源为 HCLK/8 (即 9MHz，如果系统时钟为 72MHz)
    // 这样 SysTick 每增加 1 代表 1/9 us
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    fac_us = SystemCoreClock / 8000000;  // SystemCoreClock/8 为 SysTick 时钟频率，单位 MHz
}

/**
 * @brief  微秒级延时
 * @param  us: 延时微秒数
 * @retval None
 */
void Delay_us(uint32_t us)
{
    uint32_t temp;

    // 重装载值 = 延时时间 * SysTick 时钟频率 (MHz)
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0;                      // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  // 使能 SysTick

    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));  // 等待计数到 0

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  // 关闭 SysTick
    SysTick->VAL = 0;                           // 清空计数器
}

/**
 * @brief  毫秒级延时
 * @param  ms: 延时毫秒数
 * @retval None
 */
void Delay_ms(uint32_t ms)
{
    while (ms--) {
        Delay_us(1000);
    }
}
#include "timer.h"

volatile uint32_t systick_counter = 0;

void SysTick_Init(void) {
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);
    }
}

uint32_t GetTick(void) {
    return systick_counter;
}

void Timer0_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM2, DISABLE);
}

void Timer0_SetCounter(uint16_t Value) {
    TIM_SetCounter(TIM2, Value);
}

uint16_t Timer0_GetCounter(void) {
    return TIM_GetCounter(TIM2);
}

void Timer0_Run(uint8_t Flag) {
    if (Flag) {
        TIM_Cmd(TIM2, ENABLE);
    }
    else {
        TIM_Cmd(TIM2, DISABLE);
    }
}

void delay_us(uint32_t us) {
    uint32_t i;
    for (i = 0; i < us * 6; i++) {
        __NOP();
    }
}

void delay_ms(uint32_t ms) {
    while (ms--) {
        delay_us(1000);
    }
}
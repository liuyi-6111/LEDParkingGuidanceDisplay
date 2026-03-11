#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

#define SystemCoreClock 72000000

extern volatile uint32_t systick_counter;

void Timer0_Init(void);
void Timer0_SetCounter(uint16_t Value);
uint16_t Timer0_GetCounter(void);
void Timer0_Run(uint8_t Flag);

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

void SysTick_Init(void);
uint32_t GetTick(void);

#endif
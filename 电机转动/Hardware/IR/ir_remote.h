#ifndef __IR_REMOTE_H
#define __IR_REMOTE_H

#include "stm32f10x.h"

// 红外按键定义
#define IR_CH1          0x45
#define IR_CH2          0x46
#define IR_CH3          0x47
#define IR_CH4          0x16  // 使用IR_0的键值
#define IR_PRE          0x44
#define IR_NEXT         0x40
#define IR_PLAY_PAUSE   0x43
#define IR_VOLADD       0x15
#define IR_VOLRED       0x07
#define IR_EQ           0x09
#define IR_0            0x16
#define IR_100          0x19
#define IR_200          0x0D
#define IR_1            0x0C
#define IR_2            0x18
#define IR_3            0x5E
#define IR_4            0x08
#define IR_5            0x1C
#define IR_6            0x5A
#define IR_7            0x42
#define IR_8            0x52
#define IR_9            0x4A

// 函数声明
void IR_Init(void);
uint8_t IR_GetDataFlag(void);
uint8_t IR_GetRepeatFlag(void);
uint8_t IR_GetAddress(void);
uint8_t IR_GetCommand(void);

// 定时器函数声明
void Timer0_Init(void);
void Timer0_SetCounter(uint16_t Value);
uint16_t Timer0_GetCounter(void);
void Timer0_Run(uint8_t Flag);

#endif

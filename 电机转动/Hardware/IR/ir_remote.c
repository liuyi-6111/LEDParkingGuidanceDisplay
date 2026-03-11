#include "ir_remote.h"
#include "../Timer/timer.h"

// 红外接收引脚 - PB8
#define IR_GPIO_PORT    GPIOB
#define IR_GPIO_PIN     GPIO_Pin_8
#define IR_EXTI_LINE    EXTI_Line8
#define IR_EXTI_PORT_SOURCE  GPIO_PortSourceGPIOB
#define IR_EXTI_PIN_SOURCE   GPIO_PinSource8
#define IR_EXTI_IRQn     EXTI9_5_IRQn

// 全局变量
volatile uint16_t IR_Time;
volatile uint8_t IR_State;
volatile uint8_t IR_Data[4];
volatile uint8_t IR_pData;
volatile uint8_t IR_DataFlag;
volatile uint8_t IR_RepeatFlag;
volatile uint8_t IR_Address;
volatile uint8_t IR_Command;

// 红外初始化
void IR_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIO和AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // 配置PB8为输入上拉
    GPIO_InitStructure.GPIO_Pin = IR_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(IR_GPIO_PORT, &GPIO_InitStructure);

    // 配置EXTI线
    GPIO_EXTILineConfig(IR_EXTI_PORT_SOURCE, IR_EXTI_PIN_SOURCE);

    EXTI_InitStructure.EXTI_Line = IR_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = IR_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 初始化定时器
    Timer0_Init();

    // 初始化状态
    IR_State = 0;
    IR_pData = 0;
    IR_DataFlag = 0;
    IR_RepeatFlag = 0;
}

// 获取数据标志
uint8_t IR_GetDataFlag(void) {
    if (IR_DataFlag) {
        IR_DataFlag = 0;
        return 1;
    }
    return 0;
}

// 获取重复标志
uint8_t IR_GetRepeatFlag(void) {
    if (IR_RepeatFlag) {
        IR_RepeatFlag = 0;
        return 1;
    }
    return 0;
}

// 获取地址
uint8_t IR_GetAddress(void) {
    return IR_Address;
}

// 获取命令
uint8_t IR_GetCommand(void) {
    return IR_Command;
}

// 外部中断处理函数
void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(IR_EXTI_LINE) != RESET) {
        // 红外信号处理
        if (IR_State == 0) {
            Timer0_SetCounter(0);
            Timer0_Run(1);
            IR_State = 1;
        }
        else if (IR_State == 1) {
            IR_Time = Timer0_GetCounter();
            Timer0_SetCounter(0);

            // Start信号
            if (IR_Time > 12442 - 500 && IR_Time < 12442 + 500) {
                IR_State = 2;
            }
            // Repeat信号
            else if (IR_Time > 10368 - 500 && IR_Time < 10368 + 500) {
                IR_RepeatFlag = 1;
                Timer0_Run(0);
                IR_State = 0;
            }
            else {
                IR_State = 1;
            }
        }
        else if (IR_State == 2) {
            IR_Time = Timer0_GetCounter();
            Timer0_SetCounter(0);

            // 数据0
            if (IR_Time > 1032 - 500 && IR_Time < 1032 + 500) {
                IR_Data[IR_pData / 8] &= ~(0x01 << (IR_pData % 8));
                IR_pData++;
            }
            // 数据1
            else if (IR_Time > 2074 - 500 && IR_Time < 2074 + 500) {
                IR_Data[IR_pData / 8] |= (0x01 << (IR_pData % 8));
                IR_pData++;
            }
            else {
                IR_pData = 0;
                IR_State = 1;
            }

            if (IR_pData >= 32) {
                IR_pData = 0;
                if ((IR_Data[0] == (uint8_t)~IR_Data[1]) &&
                    (IR_Data[2] == (uint8_t)~IR_Data[3])) {
                    IR_Address = IR_Data[0];
                    IR_Command = IR_Data[2];
                    IR_DataFlag = 1;
                }
                Timer0_Run(0);
                IR_State = 0;
            }
        }

        EXTI_ClearITPendingBit(IR_EXTI_LINE);
    }
}

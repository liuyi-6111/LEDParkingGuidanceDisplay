#ifndef __ADC_SENSOR_H
#define __ADC_SENSOR_H

#include "stm32f10x.h"

// 模拟传感器引脚定义
#define THERMAL_ADC_CH    ADC_Channel_0  // PA0 - 热敏电阻
#define LIGHT_ADC_CH      ADC_Channel_1  // PA1 - 光敏电阻

// 函数声明
void ADC_Sensor_Init(void);
uint16_t ADC_GetValue(uint8_t channel);
uint16_t Get_ThermalValue(void);    // 获取热敏电阻ADC值
uint16_t Get_LightValue(void);       // 获取光敏电阻ADC值

#endif
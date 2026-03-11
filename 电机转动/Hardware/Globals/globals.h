#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "stm32f10x.h"

// 全局变量声明
extern uint16_t d;
extern uint8_t modeflag;

// 传感器数据
extern uint8_t temperature;      // DHT11温度
extern uint8_t humidity;         // DHT11湿度
extern uint16_t thermal_value;   // 热敏电阻ADC值
extern uint16_t light_value;     // 光敏电阻ADC值

#endif
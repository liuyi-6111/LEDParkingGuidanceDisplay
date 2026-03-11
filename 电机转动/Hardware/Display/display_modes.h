#ifndef __DISPLAY_MODES_H
#define __DISPLAY_MODES_H

#include "stm32f10x.h"

// 圆形显示模式
void mode_circle_rotate(void);          // 模式1：旋转显示
void mode_circle_static(void);          // 模式2：静态显示
void mode_circle_text_rotate(void);     // 模式3：文字旋转
void mode_circle_pattern(void);         // 模式4：图案旋转

// 圆形显示数据（外部可访问）
extern uint8_t circle_display[360][8];  // 360度每度的显示数据

// 初始化圆形显示数据
void Init_Circle_Data(void);

#endif
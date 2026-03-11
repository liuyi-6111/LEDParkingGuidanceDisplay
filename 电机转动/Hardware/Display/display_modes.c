#include "display_modes.h"
#include "../WS2812B/ws2812b.h"
#include "../Motor/motor.h"
#include "../IR/ir_remote.h"  // 添加这行！解决IR_GetDataFlag未声明
#include <stdlib.h>
#include <math.h>

// 圆形显示数据：360个角度，每个角度8行数据
uint8_t circle_display[360][8];

// 外部函数声明
extern int16_t Encoder_GetCount(void);
extern void Encoder_Reset(void);
extern uint16_t num_leds;
extern uint8_t modeflag;  // 添加这行！声明modeflag变量

// 初始化圆形显示数据
void Init_Circle_Data(void) {
    // ... 保持不变 ...
    for (int angle = 0; angle < 360; angle++) {
        for (int row = 0; row < 8; row++) {
            circle_display[angle][row] = 0x00;
        }
    }

    // 示例1：半圆亮、半圆灭（红色）
    for (int angle = 0; angle < 180; angle++) {
        circle_display[angle][0] = 1;
    }

    // 放射状线条（绿色）
    for (int angle = 0; angle < 360; angle += 30) {
        circle_display[angle][0] = 2;
    }

    // 渐变环（蓝色渐变）
    for (int angle = 0; angle < 360; angle++) {
        int bright_rows = (angle / 45) % 8;
        circle_display[angle][0] = 3;
        circle_display[angle][1] = bright_rows;
    }
}

// 根据角度和标记设置WS2812B颜色
static void set_leds_by_angle(uint16_t angle) {
    uint8_t pattern_type = circle_display[angle][0];
    uint8_t param = circle_display[angle][1];

    for (int i = 0; i < num_leds; i++) {
        int led_angle = (i * 360 / num_leds) % 360;

        if (pattern_type == 1) { // 半圆亮
            if (angle < 180) {
                WS2812B_SetLED(i, 255, 0, 0);
            }
            else {
                WS2812B_SetLED(i, 0, 0, 0);
            }
        }
        else if (pattern_type == 2) { // 放射线条
            if (led_angle % 30 == 0) {
                WS2812B_SetLED(i, 0, 255, 0);
            }
            else {
                WS2812B_SetLED(i, 0, 0, 0);
            }
        }
        else if (pattern_type == 3) { // 渐变环
            uint8_t brightness = param * 50;
            WS2812B_SetLED(i, 0, 0, brightness);
        }
        else {
            WS2812B_SetLED(i, 0, 0, 0);
        }
    }
}

// 模式1：旋转显示
void mode_circle_rotate(void) {
    uint16_t last_angle = 0;
    int16_t encoder_val;  // 改为有符号类型
    uint16_t current_angle = 0;

    Motor_SetSpeed(40);
    Encoder_Reset();

    while (1) {
        encoder_val = Encoder_GetCount();
        // 处理负数的情况
        if (encoder_val < 0) {
            current_angle = (360 - ((-encoder_val) % 360)) % 360;
        }
        else {
            current_angle = encoder_val % 360;
        }

        if (current_angle != last_angle) {
            set_leds_by_angle(current_angle);
            WS2812B_Update();
            last_angle = current_angle;
        }

        if (IR_GetDataFlag()) {
            modeflag = 1;
            break;
        }
    }
}

// 模式2：静态显示
void mode_circle_static(void) {
    Motor_Stop();

    for (int i = 0; i < num_leds; i++) {
        uint8_t r = (i * 50) % 256;
        uint8_t g = (i * 80) % 256;
        uint8_t b = (i * 110) % 256;
        WS2812B_SetLED(i, r, g, b);
    }
    WS2812B_Update();

    while (1) {
        if (IR_GetDataFlag()) {
            modeflag = 2;
            break;
        }
    }
}

// 模式3：文字旋转
void mode_circle_text_rotate(void) {
    uint16_t last_angle = 0;
    int16_t encoder_val;
    uint16_t current_angle = 0;

    Motor_SetSpeed(35);
    Encoder_Reset();

    while (1) {
        encoder_val = Encoder_GetCount();
        if (encoder_val < 0) {
            current_angle = (360 - ((-encoder_val) % 360)) % 360;
        }
        else {
            current_angle = encoder_val % 360;
        }

        if (current_angle != last_angle) {
            for (int i = 0; i < num_leds; i++) {
                if (current_angle < 120) {
                    WS2812B_SetLED(i, 255, 0, 0);
                }
                else if (current_angle < 240) {
                    WS2812B_SetLED(i, 0, 255, 0);
                }
                else {
                    WS2812B_SetLED(i, 0, 0, 255);
                }
            }
            WS2812B_Update();
            last_angle = current_angle;
        }

        if (IR_GetDataFlag()) {
            modeflag = 3;
            break;
        }
    }
}

// 模式4：图案旋转
void mode_circle_pattern(void) {
    uint16_t last_angle = 0;
    int16_t encoder_val;
    uint16_t current_angle = 0;
    uint8_t pattern_phase = 0;

    Motor_SetSpeed(45);
    Encoder_Reset();

    while (1) {
        encoder_val = Encoder_GetCount();
        if (encoder_val < 0) {
            current_angle = (360 - ((-encoder_val) % 360)) % 360;
        }
        else {
            current_angle = encoder_val % 360;
        }

        if (current_angle != last_angle) {
            for (int i = 0; i < num_leds; i++) {
                int distance = abs(i - (int)num_leds / 2);
                if (distance < pattern_phase) {
                    uint8_t hue = (i * 10 + current_angle / 2) % 256;
                    WS2812B_SetLED(i, hue, 255 - hue, 128);
                }
                else {
                    WS2812B_SetLED(i, 0, 0, 0);
                }
            }
            WS2812B_Update();
            last_angle = current_angle;
        }

        if (current_angle == 0 && last_angle > 350) {
            pattern_phase++;
            if (pattern_phase >= num_leds / 2) pattern_phase = 0;
        }

        if (IR_GetDataFlag()) {
            modeflag = 4;
            break;
        }
    }
}
/*
 * POV LED显示数据
 * 生成时间: 2026-03-17 19:31:18
 * LED数量: 8
 * 角度采样数: 360
 */

#ifndef __LED_DATA_H
#define __LED_DATA_H

#include <stdint.h>

#define POV_NUM_LEDS 8
#define POV_NUM_ANGLES 360

const uint8_t pov_image_data[POV_NUM_ANGLES][POV_NUM_LEDS][3] = {

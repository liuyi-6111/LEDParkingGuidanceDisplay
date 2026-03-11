#include "max7219.h"

// 全局变量定义
uint16_t d = 10;
uint8_t modeflag = 0;
uint8_t display_buffer[8] = { 0 };

// 初始化GPIO
static void MAX7219_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置PA4, PA5, PA7为推挽输出
    GPIO_InitStructure.GPIO_Pin = MAX7219_CS_PIN | MAX7219_CLK_PIN | MAX7219_DIN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始状态
    GPIO_SetBits(MAX7219_CS_PORT, MAX7219_CS_PIN);
    GPIO_ResetBits(MAX7219_CLK_PORT, MAX7219_CLK_PIN);
    GPIO_ResetBits(MAX7219_DIN_PORT, MAX7219_DIN_PIN);
}

// 写入一个字节到MAX7219（MSB first）
void MAX7219_WriteByte(uint8_t data) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        GPIO_ResetBits(MAX7219_CLK_PORT, MAX7219_CLK_PIN);  // CLK低电平

        if (data & 0x80)  // 先发送高位
            GPIO_SetBits(MAX7219_DIN_PORT, MAX7219_DIN_PIN);
        else
            GPIO_ResetBits(MAX7219_DIN_PORT, MAX7219_DIN_PIN);

        GPIO_SetBits(MAX7219_CLK_PORT, MAX7219_CLK_PIN);   // CLK上升沿，数据写入
        data <<= 1;
    }
}

// 向MAX7219写入地址和数据
void MAX7219_Write(uint8_t addr, uint8_t data) {
    GPIO_ResetBits(MAX7219_CS_PORT, MAX7219_CS_PIN);  // CS低电平，开始传输
    MAX7219_WriteByte(addr);  // 先发送地址
    MAX7219_WriteByte(data);  // 再发送数据
    GPIO_SetBits(MAX7219_CS_PORT, MAX7219_CS_PIN);   // CS高电平，数据锁存
}

// MAX7219初始化
void MAX7219_Init(void) {
    MAX7219_GPIO_Init();

    // 初始化配置
    MAX7219_Write(0x09, 0x00);  // 解码模式：无解码
    MAX7219_Write(0x0A, 0x05);  // 亮度：0x00-0x0F，这里设中等亮度
    MAX7219_Write(0x0B, 0x07);  // 扫描限制：8行全部扫描
    MAX7219_Write(0x0C, 0x01);  // 正常操作模式
    MAX7219_Write(0x0F, 0x00);  // 显示测试：关闭

    MAX7219_Clear();  // 清屏
}

// 设置亮度
void MAX7219_SetBrightness(uint8_t brightness) {
    if (brightness > 0x0F) brightness = 0x0F;
    MAX7219_Write(0x0A, brightness);
}

// 清屏
void MAX7219_Clear(void) {
    uint8_t i;
    for (i = 1; i <= 8; i++) {
        MAX7219_Write(i, 0x00);  // 地址1-8对应行1-8
        display_buffer[i - 1] = 0;
    }
}

// 设置某一行数据
void MAX7219_SetRow(uint8_t row, uint8_t data) {
    if (row >= 1 && row <= 8) {
        display_buffer[row - 1] = data;
        MAX7219_Write(row, data);
    }
}

// 微秒级延时
void delay_us(uint32_t us) {
    uint32_t i;
    for (i = 0; i < us * 6; i++) {
        __NOP();
    }
}

// 毫秒级延时
void delay_ms(uint32_t ms) {
    while (ms--) {
        delay_us(1000);
    }
}

// 兼容原led_delay
void led_delay(uint16_t xus) {
    delay_us(xus * 50);
}

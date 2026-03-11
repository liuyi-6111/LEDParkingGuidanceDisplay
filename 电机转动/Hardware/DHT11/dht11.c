#include "dht11.h"
#include "../Timer/timer.h"  // 使用已有的延时函数

// 设置GPIO为输出模式
static void DHT11_SetOutput(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

// 设置GPIO为输入模式
static void DHT11_SetInput(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

// DHT11初始化
void DHT11_Init(void) {
    RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);
    DHT11_SetOutput();
    DHT11_OUT_HIGH();  // 初始拉高
    delay_ms(1000);    // 上电稳定
}

// 发送起始信号并检测响应
static uint8_t DHT11_CheckResponse(void) {
    uint8_t timeout = 0;

    // 关中断，保证时序精确
    __disable_irq();

    // 拉低至少18ms
    DHT11_SetOutput();
    DHT11_OUT_LOW();
    delay_ms(20);

    // 拉高20-40us
    DHT11_OUT_HIGH();
    delay_us(30);

    // 切换输入模式
    DHT11_SetInput();

    // 等待DHT11拉低总线（响应信号）
    timeout = 0;
    while (DHT11_IN_READ() && timeout < 100) {
        delay_us(1);
        timeout++;
    }
    if (timeout >= 100) {
        __enable_irq();
        return 0;  // 超时
    }

    // 等待DHT11拉高总线
    timeout = 0;
    while (!DHT11_IN_READ() && timeout < 100) {
        delay_us(1);
        timeout++;
    }
    __enable_irq();

    if (timeout >= 100) return 0;  // 超时
    return 1;  // 响应成功
}

// 读取一个字节
static uint8_t DHT11_ReadByte(void) {
    uint8_t i, data = 0;

    __disable_irq();

    for (i = 0; i < 8; i++) {
        // 等待50us低电平开始
        while (!DHT11_IN_READ());

        // 延时40us后判断电平
        delay_us(40);

        data <<= 1;
        if (DHT11_IN_READ()) {
            data |= 0x01;
            // 等待高电平结束
            while (DHT11_IN_READ());
        }
    }

    __enable_irq();
    return data;
}

// 读取温湿度数据
// 返回值: 0-成功, 1-失败
// temp: 温度值 (整数)
// humi: 湿度值 (整数)
uint8_t DHT11_ReadData(uint8_t* temp, uint8_t* humi) {
    uint8_t buf[5];
    uint8_t i;
    uint8_t checksum;

    // 发送起始信号并检查响应
    if (!DHT11_CheckResponse()) {
        return 1;
    }

    // 读取5个字节
    for (i = 0; i < 5; i++) {
        buf[i] = DHT11_ReadByte();
    }

    // 校验
    checksum = buf[0] + buf[1] + buf[2] + buf[3];
    if (checksum != buf[4]) {
        return 2;  // 校验失败
    }

    *humi = buf[0];  // 湿度整数
    *temp = buf[2];  // 温度整数

    return 0;
}
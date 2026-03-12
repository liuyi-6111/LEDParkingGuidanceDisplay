#include "dht11.h"
#include "delay.h"  // 如果使用SysTick延时，需要包含

/**
 * @brief  微秒级延时函数
 * @param  us: 延时微秒数
 * @retval None
 */
void DHT11_Delay_us(uint32_t us)
{
    uint32_t i;
    for (i = 0; i < (us * 8); i++) {
        __NOP();
    }
}

/**
 * @brief  DHT11 GPIO 初始化为输出模式
 * @param  None
 * @retval None
 */
static void DHT11_GPIO_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief  DHT11 GPIO 初始化为输入模式
 * @param  None
 * @retval None
 */
static void DHT11_GPIO_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief  DHT11 初始化
 * @param  None
 * @retval None
 */
void DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能 GPIO 时钟
    RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);

    // 先设置为输出模式，初始化为高电平
    DHT11_GPIO_Output();
    DHT11_OUT_H();
}

/**
 * @brief  DHT11 复位
 * @param  None
 * @retval None
 */
static void DHT11_Reset(void)
{
    DHT11_GPIO_Output();
    DHT11_OUT_L();          // 拉低至少 18ms
    DHT11_Delay_us(19000);  // 延时 19ms
    DHT11_OUT_H();          // 拉高 20-40us
    DHT11_Delay_us(30);     // 延时 30us
    DHT11_GPIO_Input();     // 设置为输入模式，等待 DHT11 响应
}

/**
 * @brief  等待 DHT11 响应
 * @param  None
 * @retval 0: 响应成功, 1: 响应失败
 */
static uint8_t DHT11_CheckResponse(void)
{
    uint8_t timeout = 0;

    // 等待 DHT11 拉低总线 (响应信号)
    while (DHT11_IN_READ() && timeout < 100) {
        timeout++;
        DHT11_Delay_us(1);
    }

    if (timeout >= 100) {
        return 1;  // 超时，无响应
    }

    timeout = 0;
    // 等待 DHT11 拉高总线 (准备发送数据)
    while (!DHT11_IN_READ() && timeout < 100) {
        timeout++;
        DHT11_Delay_us(1);
    }

    if (timeout >= 100) {
        return 1;  // 超时，响应异常
    }

    return 0;  // 响应成功
}

/**
 * @brief  从 DHT11 读取一个字节
 * @param  None
 * @retval 读取到的字节数据
 */
static uint8_t DHT11_ReadByte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++) {
        uint16_t timeout = 0;

        // 等待 50us 低电平开始
        while (!DHT11_IN_READ() && timeout < 100) {
            timeout++;
            DHT11_Delay_us(1);
        }

        timeout = 0;
        DHT11_Delay_us(40);  // 延时 40us，判断高电平持续时间

        // 如果仍然为高电平，则为数据 1，否则为 0
        if (DHT11_IN_READ()) {
            data |= (1 << (7 - i));  // 高位先出
        }

        timeout = 0;
        // 等待高电平结束
        while (DHT11_IN_READ() && timeout < 100) {
            timeout++;
            DHT11_Delay_us(1);
        }
    }

    return data;
}

/**
 * @brief  读取 DHT11 数据
 * @param  dht11_data: 存储读取到的数据
 * @retval 0: 读取成功, 1: 读取失败
 */
uint8_t DHT11_ReadData(DHT11_Data_TypeDef* dht11_data)
{
    uint8_t buffer[5] = { 0, 0, 0, 0, 0 };
    uint8_t i;
    uint8_t checksum;

    // 复位 DHT11
    DHT11_Reset();

    // 检查响应
    if (DHT11_CheckResponse()) {
        return 1;  // 响应失败
    }

    // 读取 40 位数据 (5 个字节)
    for (i = 0; i < 5; i++) {
        buffer[i] = DHT11_ReadByte();
    }

    // 校验和计算
    checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];

    // 再次设置为输出模式，拉高释放总线
    DHT11_GPIO_Output();
    DHT11_OUT_H();

    // 校验数据
    if (checksum != buffer[4]) {
        return 2;  // 校验和错误
    }

    // 数据正确，存储到结构体
    dht11_data->humidity_int = buffer[0];
    dht11_data->humidity_deci = buffer[1];
    dht11_data->temperature_int = buffer[2];
    dht11_data->temperature_deci = buffer[3];
    dht11_data->checksum = buffer[4];

    return 0;  // 读取成功
}
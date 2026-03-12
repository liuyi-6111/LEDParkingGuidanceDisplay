/**
 ******************************************************************************
 * @file    main.c
 * @author  User
 * @version V1.0
 * @date    2024
 * @brief   DHT11 温湿度传感器示例程序
 ******************************************************************************
 */

#include "stm32f10x.h"
#include "dht11.h"
#include "delay.h"

 // 调试串口 USART1 (PA9-TX, PA10-RX)
void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // USART1 TX (PA9) 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART1 RX (PA10) 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART 配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  通过 USART1 发送一个字符
 * @param  ch: 要发送的字符
 * @retval None
 */
void USART1_SendChar(uint8_t ch)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, ch);
}

/**
 * @brief  通过 USART1 发送字符串
 * @param  str: 要发送的字符串
 * @retval None
 */
void USART1_SendString(char* str)
{
    while (*str) {
        USART1_SendChar(*str++);
    }
}

/**
 * @brief  发送数字
 * @param  num: 要发送的数字
 * @retval None
 */
void USART1_SendNumber(uint32_t num)
{
    char buf[20];
    sprintf(buf, "%d", num);
    USART1_SendString(buf);
}

/**
 * @brief  发送温湿度数据
 * @param  dht11_data: DHT11 数据
 * @retval None
 */
void Display_DHT11_Data(DHT11_Data_TypeDef* dht11_data)
{
    USART1_SendString("\r\n========== DHT11 数据 ==========\r\n");
    USART1_SendString("湿度: ");
    USART1_SendNumber(dht11_data->humidity_int);
    USART1_SendChar('.');
    USART1_SendNumber(dht11_data->humidity_deci);
    USART1_SendString(" %\r\n");

    USART1_SendString("温度: ");
    USART1_SendNumber(dht11_data->temperature_int);
    USART1_SendChar('.');
    USART1_SendNumber(dht11_data->temperature_deci);
    USART1_SendString(" C\r\n");
    USART1_SendString("================================\r\n");
}

/**
 * @brief  LED 初始化 (PC13 为板载 LED)
 */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_13);  // 默认熄灭 (低电平点亮)
}

/**
 * @brief  主函数
 */
int main(void)
{
    DHT11_Data_TypeDef dht11_data;
    uint8_t status;
    uint32_t count = 0;

    // 系统初始化 (SystemInit 已在启动文件中调用)

    // 初始化外设
    Delay_Init();
    USART1_Init();
    DHT11_Init();
    LED_Init();

    USART1_SendString("\r\n\r\n========== DHT11 测试程序启动 ==========\r\n");
    USART1_SendString("MCU: STM32F103C8T6\r\n");
    USART1_SendString("DHT11 Pin: PB9\r\n");
    USART1_SendString("串口波特率: 115200\r\n\r\n");

    while (1)
    {
        // 读取 DHT11 数据
        status = DHT11_ReadData(&dht11_data);

        if (status == 0) {
            // 读取成功，LED 闪烁一次
            GPIO_ResetBits(GPIOC, GPIO_Pin_13);  // LED 亮
            Display_DHT11_Data(&dht11_data);
            Delay_ms(200);
            GPIO_SetBits(GPIOC, GPIO_Pin_13);    // LED 灭
        }
        else {
            // 读取失败
            USART1_SendString("\r\nDHT11 读取失败，错误码: ");
            USART1_SendNumber(status);
            USART1_SendString("\r\n");
        }

        count++;
        USART1_SendString("读取次数: ");
        USART1_SendNumber(count);
        USART1_SendString("\r\n");

        // DHT11 读取间隔至少 1 秒以上
        Delay_ms(2000);
    }
}
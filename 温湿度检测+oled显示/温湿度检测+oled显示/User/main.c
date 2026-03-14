#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
//#include "Serial.h"
#include "dht11.h"
#include "Key.h"
#include "Buzzer.h"

u8 temp;
u8 humi;
uint32_t Set_temp=28;
uint8_t KeyNum;

int main(void)
{
	OLED_Init();
	DHT11_Init();
	Key_Init();
	Delay_ms(1000);
  Buzzer_Init();
	while(DHT11_Init())
	{
		Delay_ms(1000);
	}
	//显示“设置温度”
	OLED_ShowChinese(1,1, 22);
	OLED_ShowChinese(1,2, 23);
	OLED_ShowChinese(1,3, 0);
	OLED_ShowChinese(1,4, 1);
	OLED_ShowNum(1,10,Set_temp,2);
	//显示“温度”
	OLED_ShowChinese(2,1, 0);
	OLED_ShowChinese(2,2, 1);
	OLED_ShowChar(2, 5, ':');
	OLED_ShowChar(2, 9, 'C');	
	//显示“湿度：”
	OLED_ShowChinese(3,1, 2);
	OLED_ShowChinese(3,2, 1);
	OLED_ShowChar(3, 5, ':');	
	OLED_ShowChar(3, 9, '%');

	KeyNum=Key_GetNum();
	while (1)
	{
		KeyNum=Key_GetNum();
		DHT11_Read_Data(&temp,&humi);
		Delay_ms(500);
		if(KeyNum == 1) //按键PB11，设置温度++
		{
			Set_temp=Set_temp+1;
			OLED_ShowNum(1,10,Set_temp,2);

		}
		if(KeyNum == 2)//按键PB1，设置温度--
		{
			Set_temp=Set_temp-1;
			OLED_ShowNum(1,10,Set_temp,2);

		}
		if(temp>Set_temp)
		{
				//显示“温度警报”
			OLED_ShowChinese(4,1, 0);
			OLED_ShowChinese(4,2, 1);
			OLED_ShowChinese(4,3, 34);
			OLED_ShowChinese(4,4, 35);
			Buzzer_ON();
		}
		else
			//显示“温度正常”
		{
			OLED_ShowChinese(4,1, 0);
			OLED_ShowChinese(4,2, 1);
			OLED_ShowChinese(4,3, 36);
			OLED_ShowChinese(4,4, 37);
			Buzzer_OFF();
		}
		 //显示温度数据
		 OLED_ShowNum(2,6,temp,2);
		//显示湿度数据
		 OLED_ShowNum(3,6,humi,2);

	}
}

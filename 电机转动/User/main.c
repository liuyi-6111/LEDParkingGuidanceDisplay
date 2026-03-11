#include "stm32f10x.h"
#include "Motor.h"

int main(void)
{
	Motor_Init();		// 电机初始化

	while (1)
	{
		// 正转 50% 速度
		Motor_SetSpeed(50);

		// 如果想测试反转，取消下面注释
		// Motor_SetSpeed(-50);

		// 如果想停止，取消下面注释
		// Motor_SetSpeed(0);
	}
}
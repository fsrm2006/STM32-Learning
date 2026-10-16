#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"InfraredSensor.h"

int main(){
	
	OLED_Init();
	IS_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	OLED_ShowString(1, 1, "Count:");
	
	while(1)
	{
		OLED_ShowNum(1, 7, IS_GetCnt(), 5);
		
		OLED_ShowString(2, 1, "Running");
		Delay_ms(50);
		OLED_ShowString(2, 1, "       ");
		
		//进入停止模式，一键配置SLEEPDEEP和PDDS参数，电压调节器开，用WFI进入停止模式
		PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);		
		
		//退出停止模式后HSI被选为系统时钟，主频变成8MHz，运行明显变慢
		//调用SystemInit()重新配置主频
		SystemInit();
	}
	

	return 0;
}

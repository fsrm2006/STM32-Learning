#include<stm32f10x.h>
#include"OLED.h"
#include"PWM.h"
#include"IC.h"

int main(){

	OLED_Init();
	PWM_Init();
	IC_init();
	
	PWM_SetPrescaler(720 - 1);	//设置PSC 频率=72M / (PSC+1) / (ARR+1)	= 1kHz
	PWM_SetCompare1(80);		//设置占空比
	OLED_ShowString(1,1,"Frequency:");
	OLED_ShowString(2,1,"00000Hz");
	OLED_ShowString(3,1,"Duty:");
	OLED_ShowString(4,1,"000%");
	
	while(1)
	{
		OLED_ShowNum(2,1, IC_GetFreq(), 5);
		OLED_ShowNum(4,1, IC_GetDuty(), 3);
	}
	return 0;
}

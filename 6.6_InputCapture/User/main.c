#include<stm32f10x.h>
#include"Delay.h"
#include"OLED.h"
#include"PWM.h"
#include"IC.h"

int main(){
	
	OLED_Init();
	PWM_Init();
	IC_init();
	
	PWM_SetPrescaler(720 - 1);	//设置PSC 频率=72M / (PSC+1) / (ARR+1)	= 1kHz
	PWM_SetCompare1(50);		
	OLED_ShowString(1,1,"Frequency:");
	OLED_ShowString(2,1,"00000Hz");
	
	while(1){
		OLED_ShowNum(2,1, IC_GetFreq(), 5);
	}
	return 0;
}

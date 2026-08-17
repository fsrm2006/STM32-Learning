#include<stm32f10x.h>
#include"OLED.h"
#include"Encoder.h"
#include"Delay.h"
#include"Timer.h"

int main(){
	

	OLED_Init();
	Encoder_Init();
	Timer_Init();
	
	OLED_ShowString(1,1,"Speed: ");

	while(1)
	{
		
	}
	return 0;
}

//使用TIM中断防止主程序阻塞
void TIM2_IRQHandler(){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)	//检查中断标志位
	{
		OLED_ShowSignedNum(2,1,Encoder_GetSpeed(),5);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
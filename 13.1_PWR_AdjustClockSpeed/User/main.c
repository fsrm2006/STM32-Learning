#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"


int main(){
	
OLED_Init();
	//显示主频；
	OLED_ShowString(1, 1, "SYSCLK:");
	OLED_ShowNum(1, 8, SystemCoreClock, 8);
	
	
	while(1)
	{
		OLED_ShowString(2, 1, "Running");//每过1s闪烁，用来具象化主频变化
		
		Delay_ms(500);						//Delay随主频变化延时产生变化，36MHz下500ms是1s
		
		OLED_ShowString(2, 1, "       ");
		
		Delay_ms(500);					//修改主频后和主频相关的计时操作记得一起修改
	}

	return 0;
}

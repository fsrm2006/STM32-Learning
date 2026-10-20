#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"Key.h"
#include"MyIWDG.h"

int main(){
	
	OLED_Init();
	Key_Init();
	
	OLED_ShowString(1, 1, "IWDG TEST!");
	
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET){	//查看上次复位是否由看门狗引起
		OLED_ShowString(2, 1, "IWDGRST!");
		Delay_ms(500);
		OLED_ShowString(2, 1, "        ");
		RCC_ClearFlag();		//IWDGRST标志位不会自动清除，需手动清除
	}
	else{
		OLED_ShowString(2, 1, "RST!");
		Delay_ms(500);
		OLED_ShowString(2, 1, "    ");
	}
	
	
	MyIWDG_Init();
	
	while(1)
	{
		MyIWDG_FeedDog();	//喂狗
		
		OLED_ShowString(3, 1, "FEED!");
		Delay_ms(200);
		OLED_ShowString(3, 1, "     ");
		Delay_ms(600);
		
		//PB1按钮长按阻塞程序，触发看门狗复位
		Key_GetNum();
	}
	

	return 0;
}

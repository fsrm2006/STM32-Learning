#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"Key.h"
#include"MyWWDG.h"

int main(){
	
	OLED_Init();
	Key_Init();
	
	OLED_ShowString(1, 1, "WWDG TEST!");
	
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET){	//查看上次复位是否由看门狗引起
		OLED_ShowString(2, 1, "WWDGRST!");
		Delay_ms(500);
		OLED_ShowString(2, 1, "        ");
		RCC_ClearFlag();		//WWDGRST标志位不会自动清除，需手动清除
	}
	else{
		OLED_ShowString(2, 1, "RST!");
		Delay_ms(500);
		OLED_ShowString(2, 1, "    ");
	}
	
	
	MyWWDG_Init();	//初始化看门狗
	
	while(1)
	{
		//PB1按钮长按阻塞程序，触发看门狗复位
		Key_GetNum();
		
		//正常喂狗
		OLED_ShowString(3, 1, "FEED!");
		Delay_ms(20);
		OLED_ShowString(3, 1, "     ");
		Delay_ms(20);
		
//		//过快喂狗(快于30ms)
//		OLED_ShowString(3, 1, "     ");
//		OLED_ShowString(3, 1, "FEED!");
//		Delay_ms(10);
//		OLED_ShowString(3, 1, "     ");
//		Delay_ms(10);
		
		MyWWDG_FeedDog();	//喂狗，要和MyWWDG_Init最后的WWDG_Enable函数保持进入窗口的合适间隔防止复位


	return 0;
}

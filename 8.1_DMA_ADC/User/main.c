#include<stm32f10x.h>
#include"OLED.h"
#include"AD.h"
#include"Delay.h"

int main(){
	OLED_Init();
	AD_Init();
	
	OLED_ShowString(1, 1, "AD0: ");
	OLED_ShowString(2, 1, "AD1: ");
	OLED_ShowString(3, 1, "AD2: ");
	OLED_ShowString(4, 1, "AD3: ");
	while(1)
	{
		for (int i = 1; i <= 4; i++){
			OLED_ShowNum(i, 6, AD[i-1], 4);
		}
		Delay_ms(100);
	}
	return 0;
}

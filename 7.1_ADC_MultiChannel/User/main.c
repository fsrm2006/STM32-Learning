#include<stm32f10x.h>
#include"OLED.h"
#include"AD.h"
#include"Delay.h"

uint16_t AD[4];
int main(){
	OLED_Init();
	AD_Init();
	
	OLED_ShowString(1, 1, "AD0: ");
	OLED_ShowString(2, 1, "AD1: ");
	OLED_ShowString(2, 1, "AD2: ");
	OLED_ShowString(2, 1, "AD3: ");
	while(1)
	{
		AD[0] =AD_GetValue(ADC_Channel_0);
		AD[1]= AD_GetValue(ADC_Channel_1);
		AD[2]= AD_GetValue(ADC_Channel_2);
		AD[3]= AD_GetValue(ADC_Channel_3);
		
		for (int i = 1; i <= 4; i++){
			OLED_ShowNum(i, 6, AD[i-1], 4);
		}
		Delay_ms(300);
	}
	return 0;
}

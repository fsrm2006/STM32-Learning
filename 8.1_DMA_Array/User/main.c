#include<stm32f10x.h>
#include"OLED.h"
#include"MyDMA.h"
#include"Delay.h"

uint8_t DataA[] = {0x1, 0x2, 0x3, 0x4};
uint8_t DataB[] = {0, 0, 0, 0};
int main(){
	OLED_Init();
	MyDMA_Init((uint32_t)DataA, (uint32_t)DataB, 4);
	OLED_ShowString(1,1,"DataA:");
	OLED_ShowString(3,1,"DataB:");
	OLED_ShowHexNum(1,7,(uint32_t)DataA,8);
	OLED_ShowHexNum(3,7,(uint32_t)DataB ,8);

	while(1)
	{
		for (int i = 0; i < 4; i++){
			DataA[i]++;
		}
		for (int i = 0; i < 4; i++){
			OLED_ShowHexNum(2,i * 3 + 1,DataA[i], 2);
		}//显示A
		
		Delay_ms(1000);
		
		MyDMA_Transfer();
		
		for (int i = 0; i < 4; i++){
			OLED_ShowHexNum(4,i * 3 + 1,DataB[i], 2);
		}//显示B
		
		Delay_ms(1000);
	}
	return 0;
}

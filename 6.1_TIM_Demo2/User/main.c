#include<stm32f10x.h>
#include"Delay.h"
#include"OLED.h"
#include"Timer.h"

uint16_t Num = 0;

uint8_t Num_offset = 6;
int main(){
	int len = 0;
	OLED_Init();
	Timer_Init();
	OLED_ShowString(1,1,"Time: ");
	OLED_ShowString(2,1,"CNT: ");
	
	while(1){
		int temp = Num;
		len = 0;
		while (temp){
			len++;
			temp/=10;
		}
		if (Num == 0) len = 1;
		OLED_ShowNum(1, Num_offset + 1, Num, len);
		OLED_ShowNum(2, 6, Timer_GerCounter(), 5);
	}
	return 0;
}
 
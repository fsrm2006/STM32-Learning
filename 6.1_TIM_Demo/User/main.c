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
	
	while(1){
		int temp = Num;
		len = 0;
		while (temp){
			len++;
			temp/=10;
		}
		if (Num == 0) len = 1;
		OLED_ShowNum(1, Num_offset + 1, Num, len);
//		OLED_ShowNum(2, 1, TIM_GetCounter(TIM2), 5);	//手动查看TIM2CNT的值
//		for (int i=1;i<=5-len;i++){
//			OLED_ShowChar(1, Num_offset + len + i, ' ');	//填充空格，防止之前数字存留
//		}
	}
	return 0;
}
 
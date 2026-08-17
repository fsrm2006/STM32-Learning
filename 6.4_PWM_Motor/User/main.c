#include<stm32f10x.h>
#include<string.h>
#include"Delay.h"
#include"OLED.h"
#include"Key.h"
#include"Motor.h"

int8_t Speed = 0;

int main(){
	
	Key_Init();
	OLED_Init();
	Motor_Init();
	
	OLED_ShowString(1,1,"Now Speed Level:");
	Motor_SetSpeed(0);
	while(1){
	    if (Key_GetNum() == 1){
			Speed += 25;
			if (Speed > 100){
				Speed= -100;	//防止越界，并且开始反转，建议弄两个按钮单独控制转速加减
			}
		}
		Motor_SetSpeed(Speed);
		OLED_ShowSignedNum(2,1,Speed/25,1);
		
	}
	return 0;
}

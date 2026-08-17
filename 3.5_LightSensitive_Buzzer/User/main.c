#include<stm32f10x.h>
#include"Delay.h"
#include"LED.h"
#include"Key.h"
#include"Buzzer.h"
#include"LightSensor.h"

int main(){
	LED_Init();
	Key_Init();
	Buz_Init();
	LS_Init();
	uint8_t KeyNum = 0;
	uint8_t LSNum = 0;
	while(1){
		//按键控制LED
		KeyNum=Key_GetNum();
		
		switch (KeyNum){
			case 1:
				LED1_Turn();
			break;
			case 2:
				LED2_Turn();
			break;
		}
		
		//光敏控制蜂鸣器
		LSNum = LS_CheckLight();
		if (LSNum !=0 )
		{
			Buz_ON();
		}
		else
		{
			Buz_OFF();
		}
		
	}
	return 0;
}
 
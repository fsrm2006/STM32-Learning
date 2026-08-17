#include<stm32f10x.h>
#include"Delay.h"
#include"LED.h"
#include"Key.h"
int main(){
	LED_Init();
	Key_Init();
	uint8_t KeyNum = 0;
	while(1){
		KeyNum=Key_GetNum();
		
//		功能1：两个按键控制一盏灯亮灭
//		if (KeyNum == 1) {		//右按键点亮灯1
//			LED1_ON();
//		}
//		else if (KeyNum == 2){  //左按键熄灭灯1
//			LED1_OFF();
//		}
		
//		功能2：两个按键分别控制两盏灯翻转亮灭
		switch (KeyNum){
			case 1:
				LED1_Turn();
			break;
			case 2:
				LED2_Turn();
			break;
		}
	}
	return 0;
}
 
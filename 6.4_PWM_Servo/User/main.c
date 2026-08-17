#include<stm32f10x.h>
#include"Delay.h"
#include"OLED.h"
#include"PWM.h"
#include"Key.h"
#include"Servo.h"

float angle = 0;

int main(){
	
	OLED_Init();
	Key_Init();
	Servo_Init();
	
	Servo_SetAngle(90);
	OLED_ShowString(1,1,"Current angle:");
	
	while(1){
//	    if (Key_GetNum() == 1){
//			angle += 30;
//			if (angle > 180){
//				angle = 0;
//			}
//		}
//		Servo_SetAngle(angle);
//		OLED_ShowNum(2,1,angle,3);
		Servo_KeySetAngle();
	} 
	return 0;
}

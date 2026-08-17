#include<stm32f10x.h>
#include"PWM.h"
#include"OLED.h"
#include"Key.h"

static float angle = 0;		//角度值
float offset = 30;			//每次按下角度改变值
uint8_t flag = 0; 			//0代表加，1代表减

void Servo_Init(){
	PWM_Init();
	Key_Init();
	OLED_Init();
}

void Servo_SetAngle(float angle){
	PWM_SetCompare2(angle / 180 * 2000 + 500);			//角度0-180，CCR  500 - 2500
}

void Servo_KeySetAngle(){
		if (angle >= 180){								//防止越界
			flag = 1;
		}
		else if (angle <= 0){						//防止越界
			flag = 0;
		}
		if (Key_GetNum() == 1){
			angle += flag ? -offset : offset;			//加或减角度值
		}
		
		uint16_t temp = angle;
		
		uint8_t len = angle ? 0 : 1;	//angle为0位数为1
		
		while (temp){
			len++;
			temp/=10;
		}	//获取位数


		for (int i=1;i<=3-len;i++){
			OLED_ShowChar(2, len + i,' ');	//填充空格，防止之前数字存留
		}

		PWM_SetCompare2(angle * 2000 / 180 + 500);		//设置CCR
		
		OLED_ShowNum(2,1,angle,len);		
		
}

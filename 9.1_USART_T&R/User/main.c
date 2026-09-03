#include<stdio.h>
#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"Serial.h"

int main(){
	OLED_Init();
	Serial_Init();


	
	while(1)
	{
		//使用中断检测输入
		//Serial_ReadData();
	}
	return 0;
}

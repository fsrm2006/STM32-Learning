#include<stm32f10x.h>
#include"Delay.h"
#include"OLED.h"
#include"LED.h"
#include"InfradeSensor.h"


int main(){
	int len = 0;
	int num = 0;
	IS_Init();
	LED_Init();
	OLED_Init();
	OLED_ShowString(1,1,"Count: 0");
	
	
	while(1){
		num = GetCnt()/2;	//双边沿模式，一次挡住移开会记两次
		int temp = num;
		len = 0;
		while (temp){
			len++;
			temp/=10;
		}
		OLED_ShowNum(1,8,num,len);
		
	}
	return 0;
}
 
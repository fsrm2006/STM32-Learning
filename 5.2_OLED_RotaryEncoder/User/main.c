#include<stm32f10x.h>
#include"Delay.h"
#include"OLED.h"
#include"LED.h"
#include"RotaryEncoder.h"


int main(){
	int len = 0;
	int num = 0;
	REn_Init();
	OLED_Init();
	OLED_ShowString(1,1,"Count: +0");
	
	while(1){
		num = GetREnCnt();	//双边沿模式，一次挡住移开会记两次
		int temp = num;
		len = 0;
		while (temp){
			len++;
			temp/=10;
		}
		if (num == 0) len = 1;
		OLED_ShowSignedNum(1,8,num,len);
		for (int i=1;i<=5-len;i++){
			OLED_ShowChar(1,8 + len + i,' ');	//填充空格，防止之前数字存留
		}
	}
	return 0;
}
 
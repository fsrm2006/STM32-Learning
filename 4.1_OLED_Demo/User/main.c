#include<stm32f10x.h>
#include"Delay.h"
#include"OLED.h"

int main(){
	
	OLED_Init();
	OLED_ShowChar(1,1,'A');
	OLED_ShowString(1,3,"Hello FSRM!");
	OLED_ShowNum(2,1,2658,4);
	OLED_ShowSignedNum(2,6,12345,5);
	OLED_ShowHexNum(3,1,0x123A,4);
	OLED_ShowBinNum(4,1,0x123A,13);
	
	while(1){
		
		
	}
	return 0;
}
 
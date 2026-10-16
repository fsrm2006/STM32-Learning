#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"Serial.h"

uint8_t RxData_t;

int main(){
	
OLED_Init();
	Serial_Init();
	
	OLED_ShowString(1, 1, "RxData:");
	
	while(1)
	{
		if (Serial_GetRxFlag() == 1){	//接收到数据
			RxData_t = Serial_GetRxData();
			OLED_ShowHexNum(1, 8, RxData_t, 2);
			Serial_SendByte(RxData_t);
		}
		
		OLED_ShowString(2, 1, "Running");
		Delay_ms(200);
		OLED_ShowString(2, 1, "       ");
		Delay_ms(200);
		
		__WFI();
	}

	return 0;
}

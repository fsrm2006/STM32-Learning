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
		Delay_ms(100);
		OLED_ShowString(2, 1, "       ");

		Delay_ms(100);
		//进入睡眠模式，使用中断可唤醒（WFE使用事件唤醒），CPU断电，减少耗电。
		//无需其他配置，可配置SLEEPONEXIT设置子模式，默认模式0
		__WFI();
	}

	return 0;
}

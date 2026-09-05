#include<stdio.h>
#include<string.h>
#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"Serial.h"
#include"Key.h"
#include"LED.h"

int main(){
	OLED_Init();
	LED_Init();
	Serial_Init();

	OLED_ShowString(1, 1, "RxText:");
	OLED_ShowString(3, 1, "TxText:");
	while(1)
	{
		if (Serial_GetRxFlag() == 1){
			OLED_ShowString(2, 1, "                ");		//16个空格清空屏幕
			OLED_ShowString(2, 1, (char *)Serial_RxPacket);
			
			if (strcmp((char *)Serial_RxPacket, "LED_ON") == 0){	//串口接收到开灯
				LED1_ON();
				OLED_ShowString(4, 1, "                ");		//16个空格清空屏幕
				OLED_ShowString(4, 1, "LED_ON OK!");
				Serial_SendString("LED_ON OK!\r\n");
			}
			else if (strcmp((char *)Serial_RxPacket, "LED_OFF") == 0){	//串口接收到关灯
				LED1_OFF();
				OLED_ShowString(4, 1, "                ");		//16个空格清空屏幕
				OLED_ShowString(4, 1, "LED_OFF OK!");
				Serial_SendString("LED_OFF OK!\r\n");
			}else{		//未知指令
				OLED_ShowString(4, 1, "                ");		//16个空格清空屏幕
				OLED_ShowString(4, 1, "Unknow Command!");
				Serial_SendString("Unknow Command!\r\n");
			}
			
			Serial_RxFlag = 0;		//重置标志位
		}
	}
	return 0;
}

#include<stdio.h>
#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"Serial.h"
#include"Key.h"
#include"LED.h"

int main(){
	OLED_Init();
	Serial_Init();
	Key_Init();
	LED_Init();
	OLED_ShowString(1, 1, "TxPacket:");
	OLED_ShowString(3, 1, "RxPacket:");
	Serial_TxPacket[0] = 0x12;
	Serial_TxPacket[1] = 0x34;
	Serial_TxPacket[2] = 0x56;
	Serial_TxPacket[3] = 0x78;
	
	
	
	while(1)
	{
		if (Key_GetNum() == 1){	//PB1按键按下修改数据并发送数据包
			for (int i = 0; i < 4; i++){
				Serial_TxPacket[i]++;
			}
			Serial_SendPacket();
			for (int i = 0; i < 4; i++){
				OLED_ShowHexNum(2, 1+i*3, Serial_TxPacket[i], 2);		
			}	//显示发送数据包
		}
		if (Serial_GetRxFlag() == 1){	//收到数据包
			for (int i = 0; i < 4; i++){
				//要注意是否考虑读取一半数据被后面传输的数据包覆盖的问题
				OLED_ShowHexNum(4, 1+i*3, Serial_RxPacket[i], 2);		
			}
			Serial_RxFlag = 0;		//清空标志位
		}
	}
	return 0;
}

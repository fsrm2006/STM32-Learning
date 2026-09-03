#include<stdio.h>
#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"Serial.h"

int main(){
	OLED_Init();
	Serial_Init();

	//Serial_SendByte(0x63);
	
	char str[100] = "Hello World!\r\n";		//windows换行为\r\n！！
	//char str[100] = "你好!";		//可发送中文对应UTF-8
	Serial_SendString(str);
	
	uint8_t Data[4] = {0x12, 0x13, 0x14, 0x15};
	Serial_SendArray(Data, 4);
	
	//Serial_SendNumber(12345);
	
	printf("Num = %d\r\n", 666);
	
	//多串口下使用sprintf发送数据
	//sprintf(char [], "%d",123)可以将输出内容存到char数组中，这样其他USART直接输出这个字符串就可以输出
	//char String[100];
	//sprintf(String,"Num = %d", 666);
	//Serial_SendString(String);
	//或者自己写的Serial_printf("Num = %d", 666);
	
	while(1)
	{
		
	}
	return 0;
}

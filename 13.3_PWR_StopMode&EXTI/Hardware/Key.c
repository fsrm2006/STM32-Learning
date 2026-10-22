#include<stm32f10x.h>
#include"Delay.h"
void Key_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		//输入模式GPIO速度不会生效，没有意义
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);

}
uint8_t Key_GetNum(){
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0){			//检测按钮1按下
		Delay_ms(20);											//按键消抖
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0);	//一直检测按下状态，松开退出循环
		Delay_ms(20);											//按键消抖
		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0){			//检测按钮2
		Delay_ms(20);											//按键消抖
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0);	//检测松开
		Delay_ms(20);											//按键消抖
		KeyNum = 2;
	}
	return KeyNum;
}


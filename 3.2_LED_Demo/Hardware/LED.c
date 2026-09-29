#include<stm32f10x.h>

void LED_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);		//关灯
}

void LED_ON(){
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LED_OFF(){
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}
#include<stm32f10x.h>

void Buz_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);		//高电位关闭蜂鸣器
}

void Buz_ON(){
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void Buz_OFF(){
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}



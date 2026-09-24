#include<stm32f10x.h>

void LS_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		//输入模式无效
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

uint8_t LS_CheckLight(){
	uint8_t res = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) != 0){	//光线亮
		res = 1;
	}
	return res;
}


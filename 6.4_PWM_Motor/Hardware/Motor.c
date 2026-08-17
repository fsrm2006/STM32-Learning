#include<stm32f10x.h>
#include"PWM.h"
void Motor_SetSpeed(int8_t Speed);
void Motor_Init(){
	//PWM初始化
	PWM_Init();
	
	//初始化GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	Motor_SetSpeed(0);
}

void Motor_SetSpeed(int8_t Speed){
	if (Speed >= 0) {				//Speed正负控制旋转方向
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(Speed);
	}
	else {
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(-Speed);
	}
}

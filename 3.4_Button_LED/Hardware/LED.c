#include<stm32f10x.h>

void LED_Init(){
	//初始化GPIOA 和 端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_2);
	
}
void LED1_ON(){
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void LED1_OFF(){
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LED1_Turn(){	//翻转灯状态
	if (GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1) == 0){	//读取输出端口值，检测灯亮
		LED1_OFF();
	}else{
		LED1_ON();
	}
}

void LED2_ON(){
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void LED2_OFF(){
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

void LED2_Turn(){		//翻转灯状态
	if (GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2) == 0){	//读取输出端口值，检测灯亮
		LED2_OFF();
	}else{
		LED2_ON();
	}
}		
	
	
	
	
	
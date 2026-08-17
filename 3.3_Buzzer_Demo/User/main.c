#include<stm32f10x.h>
#include"Delay.h"
int main(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |  RCC_APB2Periph_GPIOB,ENABLE);
	//将APB2总线上的GPIOA和GPIOB的时钟开启
	
	GPIO_InitTypeDef GPIO_S;					//设置GPIO初始化结构体
	GPIO_S.GPIO_Mode = GPIO_Mode_Out_PP;		//将GPIO模式设置为推挽模式
	GPIO_S.GPIO_Pin = GPIO_Pin_All;				//将GPIO控制的引脚设置为0-15端口
 	GPIO_S.GPIO_Speed = GPIO_Speed_50MHz;		//速度设置为50Mhz
												
	GPIO_Init(GPIOA, &GPIO_S);	//初始化GPIOA
	GPIO_Init(GPIOB, &GPIO_S);	//初始化GPIOB
	
	//GPIO_SetBits(GPIOA,GPIO_Pin_0);			//将Pin0设置为低电平（一高一低有电压）
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);	//用WriteBit将Pin0设置为低电平
	//GPIO_Write(GPIOA,~0b1);		//Pin0置为低电平
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	while(1){
		//点灯
		GPIO_Write(GPIOA,~0x1);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x2);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x4);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x8);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x10);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x20);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x40);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x80);
		Delay_ms(100);
		
		GPIO_Write(GPIOA,~0);				//灯灭
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);	//蜂鸣器滴滴鸣叫
 		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
	}
	return 0;
}
 
#include<stm32f10x.h>
#include"Delay.h"
int main(){
	                                                                                                                        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//将APB2总线上的GPIOA的时钟开启
	
	GPIO_InitTypeDef GPIO_S;					//设置GPIO初始化结构体
	GPIO_S.GPIO_Mode = GPIO_Mode_Out_PP;		//将GPIO模式设置为推挽模式
	GPIO_S.GPIO_Pin = GPIO_Pin_All;				//将GPIO控制的引脚设置为0-15端口
 	GPIO_S.GPIO_Speed = GPIO_Speed_50MHz;		//速度设置为50Mhz
												
	GPIO_Init(GPIOA, &GPIO_S);					//初始化GPIOA
	
	//GPIO_SetBits(GPIOA,GPIO_Pin_0);			//将Pin0设置为低电平（一高一低有电压）
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);	//用WriteBit将Pin0设置为低电平
	//GPIO_Write(GPIOA,~0b1);		//Pin0置为低电平
	while(1){
//		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);	//亮灯
//		
//		Delay_ms(250);								//延时250ms
//		
//		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);	//灭灯
//		
//		Delay_ms(250);								//这里也要给延时！！不然灭灯马上就亮灯了
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

//交叉点灯
//		GPIO_Write(GPIOA,~0x55);
//		Delay_ms(250);
//		GPIO_Write(GPIOA,~0xAA);
//		Delay_ms(250);
	}
	return 0;
}
 
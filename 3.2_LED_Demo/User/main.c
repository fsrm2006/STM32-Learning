#include<stm32f10x.h>
#include"Delay.h"
#include"LED.h"
int main(){
	                                                                                                                        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//将APB2总线上的GPIOA的时钟开启
	
	LED_Init();
	//GPIO_SetBits(GPIOA,GPIO_Pin_0);			//将Pin0设置为低电平（一高一低有电压）
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);	//用WriteBit将Pin0设置为低电平
	//GPIO_Write(GPIOA,~0b1);		//Pin0置为低电平
	while(1){
		LED_ON();	//亮灯
		
		Delay_ms(250);								//延时250ms
		
		LED_OFF();	//灭灯
		
		Delay_ms(250);								//这里也要给延时！！不然灭灯马上就亮灯了
//		GPIO_Write(GPIOA,~0x1);
//		Delay_ms(100);
//		GPIO_Write(GPIOA,~0x2);
//		Delay_ms(100);
//		GPIO_Write(GPIOA,~0x4);
//		Delay_ms(100);
//		GPIO_Write(GPIOA,~0x8);
//		Delay_ms(100);
//		GPIO_Write(GPIOA,~0x10);
//		Delay_ms(100);
//		GPIO_Write(GPIOA,~0x20);
//		Delay_ms(100);
//		GPIO_Write(GPIOA,~0x40);
//		Delay_ms(100);
//		GPIO_Write(GPIOA,~0x80);
//		Delay_ms(100);

//交叉点灯
//		GPIO_Write(GPIOA,~0x55);
//		Delay_ms(250);
//		GPIO_Write(GPIOA,~0xAA);
//		Delay_ms(250);
	}
	return 0;
}
 
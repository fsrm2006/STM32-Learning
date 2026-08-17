#include<stm32f10x.h>

int main(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_S;
	GPIO_S.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_S.GPIO_Pin = 
	while(1){
		
	}
	return 0;
}
 
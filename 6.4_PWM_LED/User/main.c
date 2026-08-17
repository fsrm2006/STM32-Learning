#include<stm32f10x.h>
#include"Delay.h"
#include"OLED.h"
#include"PWM.h"


int main(){
	
	OLED_Init();
	PWM_Init();

	
	while(1){
		
		for (int i = 0; i <= 1000; i++){
			PWM_SetCompare1(i);
			Delay_ms(1);
		}
		for (int i = 1000; i >= 0; i--){
			PWM_SetCompare1(i);
			Delay_ms(2);
		}
		
	}
	return 0;
}

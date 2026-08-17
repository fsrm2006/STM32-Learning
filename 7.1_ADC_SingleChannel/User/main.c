#include<stm32f10x.h>
#include"OLED.h"
#include"AD.h"
#include"Delay.h"
uint16_t AD_Value = 0;
float Voltage = 0;
int main(){
	OLED_Init();
	AD_Init();
	
	OLED_ShowString(1, 1, "AD_Value = ");
	OLED_ShowString(2, 1, "Voltage = 0.00V");
	while(1)
	{
		AD_Value = AD_GetValue();
		OLED_ShowNum(1, 12, AD_Value, 4);						//显示当前数字信号值
		Voltage = (float)AD_Value / 4095 * 3.3;					//通过ADC数据计算电压
		OLED_ShowNum(2, 11, Voltage, 1);						//显示电压整数部分
		OLED_ShowNum(2, 13, (uint16_t)(Voltage*100) % 100, 2);	//显示电压小数部分
		
		Delay_ms(500);
	}
	return 0;
}

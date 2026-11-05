#include <stm32f1xx_hal.h>
#include "dma.h"
#include "adc.h"
#include "OLED.h"

uint16_t MyADC_ConvertValue[2];

//启动ADC和DMA，校准ADC
void MyADC_DMA_Start(){
	HAL_ADCEx_Calibration_Start(&hadc1);	//校准ADC
	//开始ADC转换，启动DMA搬运并指定搬运目的地，搬运数据个数2
	HAL_StatusTypeDef Status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)MyADC_ConvertValue, 2);	
	if (Status != HAL_OK){	//检测ADC DMA是否开启成功
		OLED_ShowString(3, 1, "START_DMA Error!");
	}
}

float MyADC_GetVoltage(uint8_t Channel){
	return MyADC_ConvertValue[Channel] * 3.3f / 4095.0f;
}
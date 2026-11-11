#include <stm32f1xx_hal.h>
#include "OLED.h"

#ifdef __TIM_H	//防止没生成tim.h编译报错
#include "tim.h"

int16_t Encoder_Count = 0;
int16_t Encoder_Last = 0;
int16_t Encoder_Speed = 0;


int16_t Encoder_GetCNT(){
	//TI1和TI2模式，转动一格CNT+4，需要/4得到正确次数；需要强转int16_t，正常显示负数。
	return (int16_t)__HAL_TIM_GET_COUNTER(&htim2) / 4;	
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){	//TIM定时中断处理回调函数
	//定时中断72Mhz下每0.25s触发一次
	if (htim->Instance == TIM1){	//判断定时器
		Encoder_Count = Encoder_GetCNT();
		Encoder_Speed = (Encoder_Count - Encoder_Last);		//计算速度
		Encoder_Last = Encoder_Count;
	}
}

void Encoder_Start(){
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);  // 启动编码器接口
}

void TIM1_ITStart(){		//开启TIM1定时中断
	HAL_TIM_Base_Start_IT(&htim1);
}
#endif
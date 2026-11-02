#include <stm32f1xx_hal.h>
#include"LED.h"

// LED翻转，亮灭状态互换
void LED_Toggle(){
	HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN);
}

// 点亮LED（低电平点亮）
void LED_ON(){
	HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
}

// 熄灭LED（高电平熄灭）
void LED_OFF(){
	HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET);
}

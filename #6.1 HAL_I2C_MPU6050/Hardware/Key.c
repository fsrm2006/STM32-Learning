#include<stm32f1xx_hal.h>

// 按键引脚定义
#define KEY1_PIN    GPIO_PIN_1
#define KEY1_PORT   GPIOB

#define KEY2_PIN    GPIO_PIN_11
#define KEY2_PORT   GPIOB

/**
 * @brief  按键扫描，检测KEY1、KEY2按下（等待松手才返回）
 * @retval 1：有按键按下；0：无按键按下
 * @note   低电平触发，软件延时消抖，阻塞式等待松手
 */
uint8_t Key1_Flag = 0;

uint8_t Key_GetNum(void)
{
    // 检测KEY1低电平
    if (HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN) == GPIO_PIN_RESET)
    {
        HAL_Delay(20);                          // 按下消抖
        while(HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN) == GPIO_PIN_RESET); // 阻塞等待松手
        HAL_Delay(20);                          // 松开消抖
        return 1;
    }
    // 检测KEY2低电平
    if (HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN) == GPIO_PIN_RESET)
    {
        HAL_Delay(20);                          // 按下消抖
        while(HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN) == GPIO_PIN_RESET); // 阻塞等待松手
        HAL_Delay(20);                          // 松开消抖
        return 2;
    }
    return 0;                                // 无按键按下
}

//按键消抖操作，前台检测Key1_Flag，在按键触发的操作后加上
void Key1_ITHandle(){
		HAL_Delay(20);	//按下延时消抖
		while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET);	//按下阻塞
		HAL_Delay(20);	//回弹延时消抖
		Key1_Flag = 0;	//消除标志位
		//开启中断
		__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_1);	//关闭中断按键抖动还是会设置中断位，清除残留中断标志，避免重新开中断后立刻进一次
		EXTI->IMR |= GPIO_PIN_1;	//通过操作IMR寄存器开启EXTI1中断响应
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){		//EXTI1中断处理回调函数，处理按钮按下逻辑，可写在任意位置
	//判断触发引脚
	if (GPIO_Pin == GPIO_PIN_1){	//按钮按下
		Key1_Flag = 1;
		EXTI->IMR &= ~GPIO_PIN_1; 	//~按位取反，关闭EXTI1中断响应，防止按键抖动额外进入中断
	}
}

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

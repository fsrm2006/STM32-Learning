/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "LED.h"
#include "Key.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float PWM_Freq = 0;
float PWM_Duty = 0;
uint16_t CCR1_Val = 0;
uint16_t CCR2_Val = 0;
uint16_t TIM1_CCR1 = 500;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	OLED_Init();	//初始化OLED
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);  // 启动TIM1 CH1 PWM输出，不开没输出！
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);  // 启动TIM2 CNT，CH1输入捕获与中断
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);  // 启动CH2	
  OLED_ShowString(1, 1, "Out:1kHz       %");
  OLED_ShowDecimalNum(1, 10, HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_1) / 1000.0 * 100, 2);
  OLED_ShowString(2, 1, "PWMI");
  OLED_ShowString(3, 1, "Freq:       Hz");
  OLED_ShowString(4, 1, "Duty:       %");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	if (Key1_Flag == 1){
		TIM1_CCR1+=10;	//占空比+1%
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, TIM1_CCR1);		//修改TIM1 CH1输出占空比
		OLED_ShowDecimalNum(1, 10, HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_1) / 1000.0 * 100, 2);
		Key1_ITHandle();
	}
	OLED_ShowDecimalNum(3, 7, PWM_Freq, 1);
	OLED_ShowDecimalNum(4, 7, PWM_Duty, 2);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim){
	if (htim->Instance == TIM2){
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){		//注意检测的Channel名字！！！通道一上升沿，用CCR1值测量频率
			CCR1_Val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if (CCR1_Val > 0){	
				PWM_Freq = SystemCoreClock / 72.0 / CCR1_Val;	//测周法测频，Freq=F(tim)/CCR1;
			}
		}
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){	//通道二下降沿，用CCR2值测量频率
			CCR2_Val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);	//占空比=CCR2/CCR1 * 100%
			if (CCR2_Val > 0){
				PWM_Duty = (float)CCR2_Val / CCR1_Val * 100.0;	//测量占空比
			}
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

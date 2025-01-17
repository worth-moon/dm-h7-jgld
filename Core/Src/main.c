/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "memorymap.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
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
volatile float distance;
uint8_t state;
uint64_t error_count, success_count, error_count2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void start_signal(void);
void wait_echo(void);
void compute_distance(void);
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM13_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
	delay_init(275);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,1);
	HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 1);
	  //HAL_Delay(0);
	  //HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 0);
	  //HAL_Delay(0);
	  // 
	  switch (state)
	  {
		  case 0:
			  start_signal();//启动信号
			  break;
		  case 1:
				//state = 2;
			  wait_echo();
			  break;
		  case 2:
			  compute_distance();
			  break;
		  default:
			  state = 0;  
	  }
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 17;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 1536;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void start_signal(void)
{
	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 1);
	delay_us(20);
	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 0);
	state = 1;
}

void wait_echo(void)
{
	uint32_t delay_time = 5;
	uint32_t start_time = HAL_GetTick();//ms级别的计时
	uint32_t cur_time;
	while (((cur_time = HAL_GetTick()) - start_time < delay_time) && (HAL_GPIO_ReadPin(Echo_GPIO_Port, Echo_Pin) == 0)) // 等待1ms，或是超声波响应
	{

	}

	if (cur_time - start_time >= delay_time)//如果是超时退出，则重新启动
	{
		error_count++;
		state = 0;
	}
	else//反之则进入下一步
	{
		state = 2;
		success_count++;
	}
}


// 全局变量定义
uint32_t start_time_ms;
int32_t start_time_us;
uint32_t cur_time_ms;
int32_t cur_time_us;
float echo_time_ms, echo_time_us, echo_time;

void compute_distance(void)
{
	start_time_ms = HAL_GetTick(); // ms级别的计时
	start_time_us = SysTick->VAL;
	while (HAL_GPIO_ReadPin(Echo_GPIO_Port, Echo_Pin)) // 暂时不做保护，因为当前bug主要集中在state=1的地方
	{

	};
	cur_time_ms = HAL_GetTick();
	cur_time_us = SysTick->VAL;
	//echo_time = cur_time_ms - start_time_ms;
	echo_time_ms = cur_time_ms - start_time_ms; // 单位是ms,us也许需要单独处理
	echo_time_us = -(cur_time_us - start_time_us) * 0.001f * 0.00363f;//也是ms单位,因为是向下的计数器，所以应该取反
	//echo_time = (cur_time_us - start_time_us) * 0.001f * 0.00363f; // 单位是ms
	/*
	*	1. 如果ms之差不超过1，即距离小于0.17m，则cur_us必然大于start_us,此时乘以0.00363，转化为us单位，×0.001，转为ms单位
	*		实际推测发现，也不尽是，cur_us = 11601 start_us = 163258,一旦二者相减，出现一个负数，则超出了uint32_t能表达的范围，造成溢出
	*   2. 可以确定，ms级别的计时没有问题，但是分度值在0.17m，太粗了。需要把us级别的计时独立出来，而后必然要封装一个计时函数
	*/
	//if (echo_time_ms == 0.0f && echo_time_us < 0)
	//{
	//	echo_time_us += 1.0f;
	//}
	echo_time = echo_time_ms + echo_time_us; // 单位是ms
	//if (echo_time > 1000.0f)
	//{
	//	error_count2++;	
	//}
	// distance = (float)echo_time * 0.001f * 340.0f / 2.0f; // 单位是m
	distance = (float)echo_time * 0.17;
	state = 0;
}
//测试一下git，看样子是端口号的问题，再测试一次，看样子推论是错的，还需要后续慢慢测试，测试一下上传速度。再次测试端口7890.地方
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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

#ifdef  USE_FULL_ASSERT
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

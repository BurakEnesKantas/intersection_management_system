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

/*
kuzey - north
NR,NY,NG
güney - south
doğu - east
batı - west
*/

//NORTH GROUP
#define NR_PIN GPIO_PIN_8
#define NR_PORT GPIOC
#define NY_PIN GPIO_PIN_6
#define NY_PORT GPIOC
#define NG_PIN GPIO_PIN_3
#define NG_PORT GPIOA

//SOUTH GROUP
#define SR_PIN GPIO_PIN_6
#define SR_PORT GPIOD
#define SY_PIN GPIO_PIN_12
#define SY_PORT GPIOA
#define SG_PIN GPIO_PIN_11
#define SG_PORT GPIOA

//EAST GROUP
#define ER_PIN GPIO_PIN_1
#define ER_PORT GPIOC
#define EY_PIN GPIO_PIN_0
#define EY_PORT GPIOC
#define EG_PIN GPIO_PIN_2
#define EG_PORT GPIOB

//WEST GROUP
#define WR_PIN GPIO_PIN_6
#define WR_PORT GPIOB
#define WY_PIN GPIO_PIN_15
#define WY_PORT GPIOB
#define WG_PIN GPIO_PIN_10
#define WG_PORT GPIOB


void Sequences_Mode (void)
{
	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_RESET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(4000);


	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_RESET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(2000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(1000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_RESET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(4000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_RESET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(2000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(1000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_RESET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(4000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_RESET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(2000);


	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_RESET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(1000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_RESET);
	HAL_Delay(4000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_RESET);
	HAL_Delay(2000);

	// Kuzey - North
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
	// Güney - South
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
	// Doğu - East
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
	// Batı - West
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
	HAL_Delay(1000);


}

void Flash_Mode (void)
{
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);

	HAL_Delay(250);

	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
}


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  Sequences_Mode();

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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_15|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC1 PC6 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB10 PB15 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_15|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

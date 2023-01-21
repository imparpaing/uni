/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_LENGTH 30
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t character;

// --- Reception buffer ---
uint8_t rx_buffer[BUFFER_LENGTH];
__IO uint8_t rx_empty = 0;
__IO uint8_t rx_busy = 0;

// --- Message buffer ---
uint8_t message[BUFFER_LENGTH];
__IO uint8_t message_idx = 0;
__IO uint8_t message_length = 0;

// --- Frame content ---
__IO uint8_t frame_state = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
// Print character to terminal
void uart_print(unsigned char x)
{

USART3->TDR =(x);
while(!((USART3->ISR)&USART_ISR_TC)){;}
}

uint8_t char_is_endmessage(char c)
{
	if (c == '\r' || c == '\n')
	{
		return 1;
	}
	else return 0;
}

uint8_t rx_has_data()
{
	if(rx_empty == rx_busy)
	{
		return 0;
	}
	else return 1;
}

void increase_rx_empty()
{
	rx_empty++;
	if(rx_empty>BUFFER_LENGTH)
	{
		rx_empty = 0;
	}
}

void increase_rx_busy()
{
	rx_busy++;
	if(rx_busy>BUFFER_LENGTH)
	{
		rx_busy = 0;
	}
}

// Get character from the reception buffer
uint8_t get_char()
{
	uint8_t tmp;

	tmp = rx_buffer[rx_busy];
	increase_rx_busy();
	return tmp;
}

// Get message from the reception buffer
uint16_t get_message(uint8_t *array)
{
	static uint8_t tmp_arr[BUFFER_LENGTH];
	static uint16_t idx = 0;
	__IO uint16_t message_length = 0;

	// Collect data from the reception buffer
	while(rx_has_data() == 1)
	{
		tmp_arr[idx] = get_char();

		if (char_is_endmessage(tmp_arr[idx]))
		{
			// Set character at endmessage index to null
			tmp_arr[idx] = '\0';

			// Assign collected data to passed array
			for (uint8_t i=0; i<idx; i++)
			{
				array[i] = tmp_arr[i];
			}

			message_length = idx;
			idx = 0;
			return message_length;
		}
		else
		{
			idx++;
			if(idx>BUFFER_LENGTH) return 0;
		}
	}
	return 0;
}

void turn_on_led()
{
	HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, GPIO_PIN_SET);
}

void turn_off_led()
{
	HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, GPIO_PIN_RESET);
}
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

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, &character, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t led_action;

  while (1)
  {
	if (character == '\n' || character == '\r')
	{
		message_length = get_message(message);
	}

	if (message_length > 0)
	{
		for (uint8_t i=0; i<message_length; i++)
		{
			/* Find command start */
			if (message[i] == 'L')
			{
				i++;
				frame_state = 1;
			} /* if character is 'L' */
			if (message[i] == 'I')
			{
				i++;
				frame_state = 1;
			} /* if character is 'I' */

			switch (frame_state)
			{
				case 1:
					if (message[i] == 'E' && message[i+1] == 'D')
					{
						i = i+1;
						frame_state = 2;
					} /* if 'LED' sequence found */
					else frame_state = 0;
					break;

				case 2:
					if (message[i] == '[' && message[i+1] == 'O' && message[i+2] == 'N')
					{
						i = i+2;
						led_action = 1;
						frame_state = 3;
					} /* if command 'ON' found */
					else if (message[i] == '[' && message[i+1] == 'O' && message[i+2] == 'F' && message[i+3] == 'F')
					{
						i = i+3;
						led_action = 0;
						frame_state = 3;
					}
					else frame_state = 0;
					break;

				case 3:
					if (message[i] == ']' && led_action == 0)
					{
						frame_state = 0;
						turn_off_led();
					} /* toggle LED ON */
					else if (message[i] == ']' && led_action == 1)
					{
						frame_state = 0;
						turn_on_led();
					} /* toggle LED OFF */
					else frame_state = 0;
					break;

				default: break;
			}
		}
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Blue_Pin */
  GPIO_InitStruct.Pin = LED_Blue_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_Blue_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Send character to terminal
	uart_print(character);

	// Handle USART3 reception callback
	if(huart->Instance == USART3)
	{
		// Store character in reception buffer
		rx_buffer[rx_empty] = character;

		// Increase empty index
		increase_rx_empty();

		// Await next character
		HAL_UART_Receive_IT(&huart3, &character, 1);
	}
}
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

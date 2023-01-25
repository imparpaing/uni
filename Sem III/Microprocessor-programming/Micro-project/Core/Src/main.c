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
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_FRAME_LENGTH 526
// Length == MAX_FRAME_LENGTH + 10% margin
#define BUFFER_LENGTH 30
/* BUFFER LENGTH FOR DEBUGGING */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t character;

// --- Message array ----
uint8_t message[BUFFER_LENGTH];
__IO uint16_t message_length;
__IO uint16_t message_idx;

// --- Frame content ---
uint8_t sender[3];
uint8_t receiver[3];
char command_chars[3];
uint16_t command_length;
uint8_t data[512];
uint8_t checksum[3];

// --- Reception Buffer ---
uint8_t rx_buffer[BUFFER_LENGTH];
__IO uint16_t rx_empty = 0;
__IO uint16_t rx_busy = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Print single character to terminal
void uart_print(unsigned char x)
{
	USART3->TDR = (x);
	while(!((USART3->ISR)&USART_ISR_TC)){;}
}

// --- Reception ---
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

// Get single character from the reception buffer
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

// Analyze frame content
uint16_t analyze_frame(uint8_t *message)
{
	/* Store last analyzed char position
	Set index to 1 to skip starting char */
	uint16_t collection_index = 1;

	// Get sender
	for (uint8_t i=0; i<3; i++)
	{
		sender[i] = message[collection_index];
		collection_index++;
	}

	// Get receiver
	for (uint8_t i=0; i<3; i++)
	{
		receiver[i] = message[collection_index];
		collection_index++;
	}

	// Get command length
	for (uint8_t i=0; i<3; i++)
	{
		command_chars[i] = message[collection_index];
		collection_index++;
	}

	// Return command length
	// Use length to get characters from 'data' array in next step
//	command_length = strtol(command_chars, &str_ptr, 10);
	command_length = atoi(command_chars);

	// Get data
	for (uint16_t i=0; i<command_length; i++)
	{
		data[i] = message[collection_index];
		collection_index++;
	}

	// Get checksum
	for (uint8_t i=0; i<3; i++)
	{
		checksum[i] = message[collection_index];
		collection_index++;
	}
	return 0;
}
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
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, &character, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Retrieve the message
	  if (char_is_endmessage(character))
	  {
		  message_length = get_message(message);
		  analyze_frame(message);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_Blue_GPIO_Port, LED_Blue_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_button_Pin */
  GPIO_InitStruct.Pin = B1_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Blue_Pin */
  GPIO_InitStruct.Pin = LED_Blue_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_Blue_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
// Collection callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Print character to terminal
	uart_print(character);

	// Check for correct USART port
	if(huart->Instance == USART3)
	{
		// Collect character to reception buffer
		rx_buffer[rx_empty] = character;

		// Increase rx_empty index
		increase_rx_empty();

		// Continue data collection
		HAL_UART_Receive_IT(huart, &character, 1);
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

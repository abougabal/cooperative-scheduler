/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int taskCount;
struct task currentTask;
struct task readyQueue[255]={NULL};
struct task delayQueue[255]={NULL};
uint8_t Whole_num[2],Decumal_place[2];
Whole_num[0]=0x11; //register for the whole numbers
Decumal_place[0]=0x12; //register for the decimal place
uint8_t I2C_to_UART[] = {0,0,'.',0,0, '\r', '\n'};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t hexToAscii(uint8_t n)//4-bit hex value converted to an ascii character
{
 if (n>=0 && n<=9) n = n + '0';
 else n = n - 10 + 'A';
 return n;
}

int num_of_elements(struct task queue[]){
	int i=0;
		while(queue[i].funcName != NULL)
		{
			i++;
		}
		return i;
}
void init(){
	taskCount = 0;
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, Whole_num, 2, 10);
	HAL_I2C_Master_Transmit(&hi2c1, 0xD0, Decumal_place, 2, 10);
}

void swap(struct task *xp, struct task *yp)
{
    struct task temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void bubbleSort(struct task arr[], int n)
{
   int i, j;
   for (i = 0; i < n-1; i++)
       // Last i elements are already in place   
       for (j = 0; j < n-i-1; j++) 
           if (arr[j].priority > arr[j+1].priority)
              swap(&arr[j], &arr[j+1]);
}

void QueTask(int priority, void (*funcName)(void)){
	if(priority >= 1 && priority <= 8){
		int n = num_of_elements(readyQueue);
		if(n == 0){
			readyQueue[0].priority = priority;
			readyQueue[0].funcName = funcName;
			//taskCount++;
		}
		else{
			readyQueue[n].priority = priority;
			readyQueue[n].funcName = funcName;
			bubbleSort(readyQueue, taskCount);
			//taskCount++;
		}
	}
}

void ReRunMe(int volatile delayUnit){
	if(delayUnit == 0){
		QueTask(currentTask.priority, currentTask.funcName);
	}
	else{
		int volatile n = num_of_elements(delayQueue);
		delayQueue[n].priority = currentTask.priority;
		delayQueue[n].funcName = currentTask.funcName;
		currentTask.delay = delayUnit;
		delayQueue[n].delay = currentTask.delay;
		currentTask.currentSysTick = HAL_GetTick();
	}
}

int shift(int incr, struct task queue[], int size){
	int i = 0;
	while(i < size){
		queue[i] = queue[i+1];
		i++;
	}
	return(incr--);
}

void Dispatch(){
	int n = num_of_elements(readyQueue);
	//int n = sizeof(readyQueue);
	if(n != 0){
		void (*func)(void) = readyQueue[0].funcName;
		currentTask.funcName = readyQueue[0].funcName;
		currentTask.priority = readyQueue[0].priority;
		func();
		shift(0, readyQueue, n);
	}
}



void TaskA(){
 HAL_I2C_Master_Transmit(&hi2c1, 0xD0, Whole_num, 1, 10);
 HAL_I2C_Master_Receive(&hi2c1, 0xD1, Whole_num+1, 1, 10);
	 uint8_t num =  Whole_num[1]/10;
	 uint8_t rem =  Whole_num[1]%10;
	 temp_out[0] = hexToAscii(num);
	 temp_out[1] = hexToAscii(rem);
		
	 //Getting the decimal place	
	 HAL_I2C_Master_Transmit(&hi2c1, 0xD0, Decumal_place, 1, 10);
	 HAL_I2C_Master_Receive(&hi2c1, 0xD1, Decumal_place+1, 1, 10);
	 if(Decumal_place[1] == 0)
	 {
		 temp_out[3] = hexToAscii(0x0);
		 temp_out[4] = hexToAscii(0x0);
	 }
	 else if(Decumal_place[1] == 64)
	 {
		 temp_out[3] = hexToAscii(0x2);
		 temp_out[4] = hexToAscii(0x5);
	 }
	 else if(Decumal_place[1] == 128)
	 {
		 temp_out[3] = hexToAscii(0x5);
		 temp_out[4] = hexToAscii(0x0);
	 }
	 else if(Decumal_place[1] == 192)
	 {
		 temp_out[3] = hexToAscii(0x7);
		 temp_out[4] = hexToAscii(0x5);
	 }
		
	 // transmit temp to UART
	 HAL_UART_Transmit(&huart2,temp_out, sizeof(temp_out), 10);
}

void TaskB(){
	// do something here
	unsigned char temp[2] = {'h', 'i'};
	HAL_UART_Transmit(&huart2, temp, 1, HAL_MAX_DELAY);
	// Rerun again after 10 ticks (500 msec)
	ReRunMe(1000); 
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
	QueTask(1,TaskA);
	QueTask(2,TaskB);
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		Dispatch();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

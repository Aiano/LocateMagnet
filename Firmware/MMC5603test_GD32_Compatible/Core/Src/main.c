/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "math.h"
#include "I2C_SW.h"
#include <string.h>
#include <stdlib.h>
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
int32_t sensorValBuffer[5][5][4] = {0}; // x,y,z,Mag
uint32_t errorVal[1000];
int parameterM;
int minIndex;
uint8_t totalTestBit = 1;
SWI2C sensorList[25]={
	{GPIOA,GPIO_PIN_6,GPIOA,GPIO_PIN_0},
	{GPIOA,GPIO_PIN_6,GPIOA,GPIO_PIN_1},
	{GPIOA,GPIO_PIN_6,GPIOA,GPIO_PIN_2},
	{GPIOA,GPIO_PIN_6,GPIOA,GPIO_PIN_3},
	{GPIOA,GPIO_PIN_6,GPIOA,GPIO_PIN_4},	

	{GPIOA,GPIO_PIN_7,GPIOA,GPIO_PIN_0},
	{GPIOA,GPIO_PIN_7,GPIOA,GPIO_PIN_1},
	{GPIOA,GPIO_PIN_7,GPIOA,GPIO_PIN_2},
	{GPIOA,GPIO_PIN_7,GPIOA,GPIO_PIN_3},
	{GPIOA,GPIO_PIN_7,GPIOA,GPIO_PIN_4},
	
	{GPIOB,GPIO_PIN_0,GPIOA,GPIO_PIN_0},
	{GPIOB,GPIO_PIN_0,GPIOA,GPIO_PIN_1},
	{GPIOB,GPIO_PIN_0,GPIOA,GPIO_PIN_2},
	{GPIOB,GPIO_PIN_0,GPIOA,GPIO_PIN_3},
	{GPIOB,GPIO_PIN_0,GPIOA,GPIO_PIN_4},
	
	{GPIOB,GPIO_PIN_1,GPIOA,GPIO_PIN_0},
	{GPIOB,GPIO_PIN_1,GPIOA,GPIO_PIN_1},
	{GPIOB,GPIO_PIN_1,GPIOA,GPIO_PIN_2},
	{GPIOB,GPIO_PIN_1,GPIOA,GPIO_PIN_3},
	{GPIOB,GPIO_PIN_1,GPIOA,GPIO_PIN_4},
		
	{GPIOB,GPIO_PIN_2,GPIOA,GPIO_PIN_0},
	{GPIOB,GPIO_PIN_2,GPIOA,GPIO_PIN_1},
	{GPIOB,GPIO_PIN_2,GPIOA,GPIO_PIN_2},
	{GPIOB,GPIO_PIN_2,GPIOA,GPIO_PIN_3},
	{GPIOB,GPIO_PIN_2,GPIOA,GPIO_PIN_4}

};

uint8_t txBuffer[1000];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// read who-am-i register
int testSensor(SWI2C sensorLabel){
	
	i2c_start(sensorLabel);
	i2c_send(sensorLabel,0x60); // Dummy Write 0x30 1
	if(i2c_wit_ack(sensorLabel)) printf("No ACK\n");
	i2c_send(sensorLabel,0x39);
	if(i2c_wit_ack(sensorLabel)) printf("No ACK\n");
	 
	delay_us(50);
	i2c_start(sensorLabel);
	i2c_send(sensorLabel,0x61); // Read 0x30 1
	if(i2c_wit_ack(sensorLabel)) printf("No ACK\n");
	uint8_t dataSent = i2c_read(sensorLabel);
	i2c_no_ack(sensorLabel);
	i2c_stop(sensorLabel);
	if(dataSent == 0x10) {
		//printf("Test Succcess\n");
		return 1;
	}
	return 0;
}

void sensorInit(SWI2C sensorLabel){
	//Enable Continuous mode 
	uint8_t dataByte = 0xff;
	i2cWriteByte(sensorLabel,0x60,0x1A,1,&dataByte);	// Write ODR 
	dataByte = 0xA0;
	i2cWriteByte(sensorLabel,0x60,0x1B,1,&dataByte);	// Write CR0
	dataByte = 0x10;
	i2cWriteByte(sensorLabel,0x60,0x1D,1,&dataByte);	// Write CR2
	
}

void readSensorData(SWI2C sensorLabel,uint32_t* dataBuf){
	uint8_t iicDataBuffer[32];
	i2cReadByte(sensorLabel,0x60,0x00,8,iicDataBuffer);
	dataBuf[0] = (uint32_t)iicDataBuffer[0] << 12 | (uint32_t)iicDataBuffer[1] << 4 |
      (uint32_t)iicDataBuffer[6] >> 4;
	dataBuf[1] = (uint32_t)iicDataBuffer[2] << 12 | (uint32_t)iicDataBuffer[3] << 4 |
      (uint32_t)iicDataBuffer[7] >> 4;
	dataBuf[2] = (uint32_t)iicDataBuffer[4] << 12 | (uint32_t)iicDataBuffer[5] << 4 |
      (uint32_t)iicDataBuffer[8] >> 4;
	dataBuf[0] -= (uint32_t)1<<19;
	dataBuf[1] -= (uint32_t)1<<19;
	dataBuf[2] -= (uint32_t)1<<19;
}

int fastAbs(int inputNum){
	return ((inputNum >> 31) | 1) * inputNum;
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_Base_Start(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	int TargetMagX,TargetMagY,TargetMagZ;	 
	int relativeX,relativeY,relativeZ;
	int minNum;
	for(int i=0;i<25;i++){
		if(testSensor(sensorList[i])==0){
			totalTestBit=0;
		}
	 }
	if(totalTestBit) 
		printf("25 Test Succcess\n");
	for(int i=0;i<25;i++)
		sensorInit(sensorList[i]);
	int testVal[3] = {0};
	int timeSpent;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	for(int i=0;i<25;i++){
		readSensorData(sensorList[i],(uint32_t*)sensorValBuffer[i/5][i%5]);
	}
	//uint8_t strLen = 
	sprintf((char *)txBuffer, "U %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n"
														"V %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n"
														"W %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
														sensorValBuffer[0][0][0],sensorValBuffer[0][1][0],sensorValBuffer[0][2][0],sensorValBuffer[0][3][0],sensorValBuffer[0][4][0],
														sensorValBuffer[1][0][0],sensorValBuffer[1][1][0],sensorValBuffer[1][2][0],sensorValBuffer[1][3][0],sensorValBuffer[1][4][0],
														sensorValBuffer[2][0][0],sensorValBuffer[2][1][0],sensorValBuffer[2][2][0],sensorValBuffer[2][3][0],sensorValBuffer[2][4][0],
														sensorValBuffer[3][0][0],sensorValBuffer[3][1][0],sensorValBuffer[3][2][0],sensorValBuffer[3][3][0],sensorValBuffer[3][4][0],
														sensorValBuffer[4][0][0],sensorValBuffer[4][1][0],sensorValBuffer[4][2][0],sensorValBuffer[4][3][0],sensorValBuffer[4][4][0],
														sensorValBuffer[0][0][1],sensorValBuffer[0][1][1],sensorValBuffer[0][2][1],sensorValBuffer[0][3][1],sensorValBuffer[0][4][1],
														sensorValBuffer[1][0][1],sensorValBuffer[1][1][1],sensorValBuffer[1][2][1],sensorValBuffer[1][3][1],sensorValBuffer[1][4][1],
														sensorValBuffer[2][0][1],sensorValBuffer[2][1][1],sensorValBuffer[2][2][1],sensorValBuffer[2][3][1],sensorValBuffer[2][4][1],
														sensorValBuffer[3][0][1],sensorValBuffer[3][1][1],sensorValBuffer[3][2][1],sensorValBuffer[3][3][1],sensorValBuffer[3][4][1],
														sensorValBuffer[4][0][1],sensorValBuffer[4][1][1],sensorValBuffer[4][2][1],sensorValBuffer[4][3][1],sensorValBuffer[4][4][1],
														sensorValBuffer[0][0][2],sensorValBuffer[0][1][2],sensorValBuffer[0][2][2],sensorValBuffer[0][3][2],sensorValBuffer[0][4][2],
														sensorValBuffer[1][0][2],sensorValBuffer[1][1][2],sensorValBuffer[1][2][2],sensorValBuffer[1][3][2],sensorValBuffer[1][4][2],
														sensorValBuffer[2][0][2],sensorValBuffer[2][1][2],sensorValBuffer[2][2][2],sensorValBuffer[2][3][2],sensorValBuffer[2][4][2],
														sensorValBuffer[3][0][2],sensorValBuffer[3][1][2],sensorValBuffer[3][2][2],sensorValBuffer[3][3][2],sensorValBuffer[3][4][2],
														sensorValBuffer[4][0][2],sensorValBuffer[4][1][2],sensorValBuffer[4][2][2],sensorValBuffer[4][3][2],sensorValBuffer[4][4][2]);
	
	HAL_UART_Transmit_DMA(&huart1, txBuffer, strlen((char *)txBuffer));
	
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
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
	int fgetc(FILE *f)
	{
	  uint8_t ch = 0;
	  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
	  return ch;
	}

	int fputc(int ch, FILE *f)
	{
	  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
	  return ch;
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

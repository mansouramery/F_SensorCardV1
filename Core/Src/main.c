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
#include "can.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ads1115.h"
#include "max31856.h"
#include "stdio.h"
#include "canfunc.h"
#include <math.h>
#include "thermocouple.h"
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
  MX_CAN_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  
  HAL_CAN_Start( & hcan);
  HAL_CAN_ActivateNotification( & hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  
  HAL_GPIO_WritePin(VE1_GPIO_Port,VE1_Pin,GPIO_PIN_SET);  // enable ADum1401
  HAL_GPIO_WritePin(CS2_GPIO_Port , CS2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CS1_GPIO_Port , CS1_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    int16_t adc_value;
    float voltage;
    HAL_Delay(1000);
    uint8_t cs = 1;
    begin(1);
    setThermocoupleType(MAX31856_ICTYPE_K,1);
    begin(2);
    setThermocoupleType(MAX31856_ICTYPE_K,2);    
    
  float f_Temp1 = 0.0;
  float f_Temp2 = 0.0;
  float f_SP = 0.0;
  float f_PR = 0.0;
  float f_PreSP = 0.0;
  float f_PrePR = 0.0;
  int Pre_Reset = 0;
  int Pre_Reset1 = 0;  
  float f_R1 = 0.0;
  float f_R2 = 0.0;  
  int i_Temp1 = 0;
  int i_Temp2 = 0;
  int i_SP = 0;
  int i_PR = 0;
  int i_R1 = 0;
  int i_R2 = 0;
  char str_Temp[9]={0};
  char str_R1[9]={0};
  char str_R2[9]={0};  
  char str_Pressure[9]={0};
  char str_Speed[9]={0};
  float G1 = 4.004;  
  float CJT = 0.0;
  float Temp = 0.0;
printf("\nStart...\n");  
  while (1)
  {
      cs = 1;        
      f_Temp1 = readThermocoupleTemperature(cs);
      CJT = readCJTemperature(cs);
      printf("CJT1:%.2f , Temp1=%0.2f\n\n",CJT,f_Temp1);
      i_Temp1 = round(f_Temp1);
      
      cs = 2;
      f_Temp2 = readThermocoupleTemperature(cs);
      printf("CJT2:%.2f , Temp2=%0.2f\n\n",readCJTemperature(cs),f_Temp2);      
      i_Temp2 = round(f_Temp2);
      
      memset(str_Temp,0,strlen(str_Temp));      
      snprintf(str_Temp, sizeof(str_Temp), "T%d,%d", i_Temp1, i_Temp2);
      printf("....STRING DATA = %s\n",str_Temp);

  uint8_t fault = readFault(cs);
  if (fault) {
    if (fault & MAX31856_FAULT_CJRANGE) printf("Cold Junction Range Fault\n");
    if (fault & MAX31856_FAULT_TCRANGE) printf("Thermocouple Range Fault\n");
    if (fault & MAX31856_FAULT_CJHIGH)  printf("Cold Junction High Fault\n");
    if (fault & MAX31856_FAULT_CJLOW)   printf("Cold Junction Low Fault\n");
    if (fault & MAX31856_FAULT_TCHIGH)  printf("Thermocouple High Fault\n");
    if (fault & MAX31856_FAULT_TCLOW)   printf("Thermocouple Low Fault\n");
    if (fault & MAX31856_FAULT_OVUV)    printf("Over/Under Voltage Fault\n");
    if (fault & MAX31856_FAULT_OPEN)    printf("Thermocouple Open Fault\n");
  }  
//..............................................................................
        // Start conversion on AIN0 vs GND
        if (ADS1115_StartConversion(ADS1115_MUX_AIN0_GND,ADS1115_ADD1,ADS1115_DR_64SPS,ADS1115_PGA_4_096V) == HAL_OK)
        {
            HAL_Delay(10);  // Wait for conversion (depends on data rate)
            
            if (ADS1115_ReadConversion(&adc_value,ADS1115_ADD1) == HAL_OK)
            {
                // Convert to voltage (depends on PGA setting)
                f_PR = (adc_value * 4.096f) / 32768.0f;
                if(f_PR> f_PrePR){
                i_PR = round(1000*G1*(f_PR));
                    f_PrePR = f_PR;
                }else{
                    i_PR = round(1000*G1*(f_PrePR));
                    Pre_Reset++;
                    HAL_GPIO_TogglePin(LED4_GPIO_Port,LED4_Pin);                     
                }
                if(Pre_Reset >200){
                   Pre_Reset = 0; 
                    f_PrePR = 0;
                }
                    
                printf("ADC1 ValueAN0: %d, Voltage: %.4fV\n", adc_value, f_PR*G1);
            }
        }

        if (ADS1115_StartConversion(ADS1115_MUX_AIN2_GND,ADS1115_ADD1,ADS1115_DR_64SPS,ADS1115_PGA_4_096V) == HAL_OK)
        {
            HAL_Delay(10);  // Wait for conversion (depends on data rate)
            
            if (ADS1115_ReadConversion(&adc_value,ADS1115_ADD1) == HAL_OK)
            {
                f_SP = (adc_value * 4.096f) / 32768.0f;
                if(f_SP >f_PreSP){
                i_SP = round(1000*G1*(f_SP));
                    f_PreSP = f_SP;
                }else{
                  i_SP = round(1000*G1*(f_PreSP));
                    Pre_Reset1++; 
                    HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);                     
                }
                if(Pre_Reset1 >200){
                    Pre_Reset1 = 0;
                    f_PreSP = 0;
                }
                
                printf("ADC1 ValueAN2: %d, Voltage: %.4fV\n", adc_value, f_SP*G1);
            }
        } 
      memset(str_Pressure,0,strlen(str_Pressure));  
      memset(str_Speed,0,strlen(str_Speed));            
      snprintf(str_Pressure, sizeof(str_Pressure), "P%d,", i_PR);
      printf("....Pressure STRING DATA = %s\n",str_Pressure);

      snprintf(str_Speed, sizeof(str_Speed), "S%d,", i_SP);
      printf("....Speed STRING DATA = %s\n",str_Speed);        
        
//.................
        if (ADS1115_StartConversion(ADS1115_MUX_AIN1_AIN3,ADS1115_ADD2, ADS1115_DR_8SPS,ADS1115_PGA_0_256V) == HAL_OK)
        {
            HAL_Delay(10);  // Wait for conversion (depends on data rate)
            
            if (ADS1115_ReadConversion(&adc_value,ADS1115_ADD2) == HAL_OK)
            {
                f_R1 = (adc_value * 0.256f) / 32768.0f;
                i_R1 = round(1*(f_R1));
                printf("ADC2 ValueAN13: %d, Voltage: %.4fV\n", adc_value, 1000.0*f_R1);
            }
        }   
        Temp = thermocouple_read_temperature(1000.0*f_R1,CJT-2.8,J_TYPE);
        printf("\nxxx.....xxx TEMP=%.3f\n",Temp);
//        if (ADS1115_StartConversion(ADS1115_MUX_AIN2_GND,ADS1115_ADD2) == HAL_OK)
//        {
//            HAL_Delay(10);  // Wait for conversion (depends on data rate)
//            
//            if (ADS1115_ReadConversion(&adc_value,ADS1115_ADD2) == HAL_OK)
//            {
//                f_R2 = (adc_value * 0.512f) / 32768.0f;
//                i_R2 = round(1*(f_R2));
//                printf("ADC2 ValueAN2: %d, Voltage: %.4fV\n", adc_value, f_R2);
//            }
//        } 
      memset(str_R1,0,strlen(str_R1));  
      memset(str_R2,0,strlen(str_R2));            
      snprintf(str_R1, sizeof(str_R1), "R%d,", i_R1);
      printf("....R1 STRING DATA = %s\n",str_R1);

      snprintf(str_R2, sizeof(str_R2), "r%d,", i_R2);
      printf("....R2 STRING DATA = %s\n",str_R2);           
        printf("\n.....................................\n");        
//..............................................................................  
      CAN_Pub(NodeSub_SensorCard1_Data, str_Temp, strlen(str_Temp));        
                HAL_Delay(1);
      CAN_Pub(NodeSub_SensorCard1_Data, str_Pressure, strlen(str_Pressure));        
                HAL_Delay(1);
      CAN_Pub(NodeSub_SensorCard1_Data, str_Speed, strlen(str_Speed));        
                HAL_Delay(1);
      CAN_Pub(NodeSub_SensorCard1_Data, str_R1, strlen(str_R1));        
                HAL_Delay(1);
      CAN_Pub(NodeSub_SensorCard1_Data, str_R2, strlen(str_R2));        
                HAL_Delay(1);        
HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);        
        HAL_Delay(80);
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
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL14;
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

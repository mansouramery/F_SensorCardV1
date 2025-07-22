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
#include "math.h"
#include "thermocouple.h"
#include "calculate_fun.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define Q_LEN 2
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
const float  offset_0 = 0.0;
const float offset_0_1 = -0.1; 
const float offset_0_5 = -0.503; 
const float offset_1_0 = -1.003; 
const float offset_4_0 = -3.003; 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int test = 0;
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
Analog thermo = {0};
Analog pressure[2] = {{0}};
Analog speed[2] = {{0}};
Analog inject_r = {0};
Analog mold_r = {0};
volatile bool cycle_sync = false;
volatile bool Send_permite = false;

Queue Qspeed = {0};
Queue Qpressure = {0};

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  //HAL_TIM_Base_Start_IT(&htim2); 
  HAL_CAN_Start( & hcan);
  HAL_CAN_ActivateNotification( & hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  
  HAL_GPIO_WritePin(VE1_GPIO_Port,VE1_Pin,GPIO_PIN_SET);  // enable ADum1401
  HAL_GPIO_WritePin(CS2_GPIO_Port , CS2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CS1_GPIO_Port , CS1_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("\nStart...\n");  
  // init the board
    max_begin(1);
    setThermocoupleType(MAX31856_ICTYPE_K,1);
    adc_init();  
    uint32_t tik = 0, tok = 0;
    VAL_opt opt;
//    strcpy(Qpressure.List[0],"-0");
//    strcpy(Qpressure.List[1],"-0");
//    strcpy(Qpressure.List[0],"-0");
//    strcpy(Qpressure.List[1],"-0");
  while (1)
  {       

////..........................................................................
if(cycle_sync == false){
    //tik = HAL_GetTick();
    if(cal_speed(&inject_r,BASE_500us) < offset_4_0){  
        //printf("\nxxxxxxxxCode here1\n");
        //printf("inject_r speed= %f\n",inject_r.minspeed);
        //printf("Value=%f,preValue=%f\n",inject_r.f_value,inject_r.f_preval);        
      ADS1115_ReadADC(&pressure[inject]); 
      ADS1115_ReadADC(&speed[inject]);
//    if(inject_r.speed < inject_r.minspeed){
//         inject_r.minspeed = inject_r.speed;
//     }    
    }    
        //tok = HAL_GetTick();
    //printf("execution_Time=%d\n",tok-tik);
    if(cal_speed(&mold_r,BASE_500us) < offset_4_0){
        //printf("\nxxxxxxxxCode here2\n");
        //printf("mold_r speed= %f\n",mold_r.minspeed);  
        //printf("Value=%f,preValue=%f\n",mold_r.f_value,mold_r.f_preval);          
      ADS1115_ReadADC(&pressure[mold]); 
      ADS1115_ReadADC(&speed[mold]);  
        
    }       
}else{
    HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
    printf("\nvvvvvvvvvvvvCode here3\n");
    //inject pressure record    
    //printf("MAX-VALUE=%f",pressure[inject].f_maxval);
    analog_record(&pressure[inject],"Ip",MAX_VAL);
    analog_reset(&pressure[inject]);
    //mold close pressure record    
    analog_record(&pressure[mold],"Mp",MAX_VAL);
    analog_queue(&Qpressure,pressure[mold].str);
    analog_reset(&pressure[mold]);     
    //inject speed record
    analog_record(&speed[inject],"Is",MAX_VAL);
    analog_reset(&speed[inject]);
    //mold close speed record    
    analog_record(&speed[mold],"Ms",MAX_VAL);
    analog_queue(&Qspeed,speed[mold].str);
    analog_reset(&speed[mold]);    
    //Inject speed record
    analog_record(&inject_r,"Rs",min_SPEED);
    analog_reset(&inject_r);   
    analog_reset(&mold_r);
    
    cycle_sync = false;
    Send_permite = true;    
}

if(Send_permite == true){
    

    pvalidation(Qpressure.List[Qpressure.Index]); 
    pvalidation(pressure[inject].str);    
    printf("pressure[inject]=%s\n",pressure[inject].str);
    printf("Qpressure.Index=%d, len=%d ,str=%s\n",Qpressure.Index,strlen(Qpressure.List[Qpressure.Index]) ,Qpressure.List[Qpressure.Index]);         
    CAN_Pub(NodeSub_SensorCard1_Data,pressure[inject].str,strlen(pressure[inject].str));
    CAN_Pub(NodeSub_SensorCard1_Data,Qpressure.List[Qpressure.Index],strlen(Qpressure.List[Qpressure.Index]));
    
    pvalidation(Qspeed.List[Qpressure.Index]); 
    pvalidation(speed[inject].str);     
    printf("speed[inject]= %s\n",speed[inject].str);
    printf("Qspeed.Index=%d,str=%s\n",Qspeed.Index,Qspeed.List[Qspeed.Index]); 
    CAN_Pub(NodeSub_SensorCard1_Data,speed[inject].str,strlen(speed[inject].str));
    CAN_Pub(NodeSub_SensorCard1_Data,Qspeed.List[Qspeed.Index],strlen(Qspeed.List[Qspeed.Index]));
    
    pvalidation(inject_r.str);       
    printf("inject_r.str= %s\n",inject_r.str);
    CAN_Pub(NodeSub_SensorCard1_Data,inject_r.str,strlen(inject_r.str));
    Send_permite = false;    
    }     
       
   //HAL_Delay(80);
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

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
#define HEX_LEN 3+1 //(3data)
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
volatile bool cycle_sync = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

Analog thermo = {0};
Analog pressure1[2] = {{0}};
Analog pressure2[2] = {{0}};
Analog inject_rs = {0};
Analog mold_rs = {0};
Analog inject_rp = {0};
Analog mold_rp = {0};
Analog injectPressure = {0};
Analog moldPressure= {0};

volatile bool Send_permite = false;

Queue Qpressure = {0};
Queue QrulerSpeed = {0};
Queue QrulerMinPos = {0};
Queue QrulerMaxPos = {0};
uint8_t dataStream[8]={0};
//Queue Qpressure2 = {0}; //speed -> pressure2

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
//    max_begin(1);
//    setThermocoupleType(MAX31856_ICTYPE_K,1);
    adc_init();  
    uint32_t tik = 0, tok = 0;
    VAL_opt opt;
//    strcpy(Qpressure.List[0],"-0");
//    strcpy(Qpressure.List[1],"-0");
//    strcpy(Qpressure.List[0],"-0");
//    strcpy(Qpressure.List[1],"-0");
  int streamValue1 = 0,streamValue2 = 0;
  uint32_t t1 = 0;
  uint32_t t2 = 0;
  while (1)
  { 
      t1 = HAL_GetTick();
//      CAN_Pub(NodePub_SensorCard1_ACK,"CARD1",5);
 /*
      ADS1115_ReadADC(&pressure1[inject]); 
      ADS1115_ReadADC(&pressure2[inject]);
      ADS1115_ReadADC(&inject_rp); 
      streamValue1 = pressure1[inject].i_value + pressure2[inject].i_value;
      streamValue2 = inject_rp.i_value;
      dataStream[0] = 'I';
      dataStream[1] = (streamValue1>>8)&0xFF; 
      dataStream[2] = streamValue1&0xFF;
      dataStream[3] = (streamValue2>>8)&0xFF; 
      dataStream[4] = streamValue2&0xFF;
      CAN_Pub(NodePub_SensorCard1_ACK,dataStream,5);
*/
      
////..........................................................................
      
 cycle_sync = false;  
Send_permite = false;      
if(cycle_sync == false){
    //tik = HAL_GetTick();
    if(cal_speed(&inject_rs,BASE_500us) < 0 ){        
      ADS1115_ReadADC(&pressure1[inject]); 
      ADS1115_ReadADC(&pressure2[inject]);
      ADS1115_ReadADC(&inject_rp); 
      streamValue1 = pressure1[inject].i_value + pressure2[inject].i_value;
      streamValue2 = inject_rp.i_value;
      dataStream[0] = 'I';
      dataStream[1] = (streamValue1>>8)&0xFF; 
      dataStream[2] = streamValue1&0xFF;
      dataStream[3] = (streamValue2>>8)&0xFF; 
      dataStream[4] = streamValue2&0xFF;
      CAN_Pub(NodePub_SensorCard1_ACK,dataStream,5);         
    } else{   
//    if(cal_speed(&mold_rs,BASE_500us) < 1){        
      ADS1115_ReadADC(&pressure1[mold]); 
      ADS1115_ReadADC(&pressure2[mold]);
      ADS1115_ReadADC(&mold_rp); 
      streamValue1 = pressure1[mold].i_value + pressure2[mold].i_value;
      streamValue2 = mold_rp.i_value;
      dataStream[0] = 'M';
      dataStream[1] = (streamValue1>>8)&0xFF; 
      dataStream[2] = streamValue1&0xFF;
      dataStream[3] = (streamValue2>>8)&0xFF; 
      dataStream[4] = streamValue2&0xFF;
      CAN_Pub(NodePub_SensorCard1_ACK,dataStream,5);        
    }
    HAL_GPIO_TogglePin(LED4_GPIO_Port,LED4_Pin);    
}else{
    HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
    
    analog_record(&pressure1[inject],"P1I",MAX_VAL); // Inject pressure(Ip)P1I
    analog_record(&pressure2[inject],"P2I",MAX_VAL);// Inject pressure2(Ip)P2I
    
    injectPressure.i_maxval = pressure1[inject].i_maxval + pressure2[inject].i_maxval;   
    snprintf(injectPressure.str, sizeof(injectPressure.str), "P%d",injectPressure.i_maxval); 
    injectPressure.dataMax_hex[0] = 'P';
    injectPressure.dataMax_hex[1] = (injectPressure.i_maxval >>8)&0xFF; 
    injectPressure.dataMax_hex[2] =  injectPressure.i_maxval&0xFF;       

    //mold close pressure1 record    
    analog_record(&pressure1[mold],"P1M",MAX_VAL);// mold pressure1(Mp)P1M
    analog_record(&pressure2[mold],"P2M",MAX_VAL);// mold pressure2(Mp)P2M 
    
    moldPressure.i_maxval = pressure1[mold].i_maxval + pressure2[mold].i_maxval;   
    snprintf(moldPressure.str, sizeof(moldPressure.str), "p%d",moldPressure.i_maxval);
    moldPressure.dataMax_hex[0] = 'p';
    moldPressure.dataMax_hex[1] = (moldPressure.i_maxval >>8)&0xFF; 
    moldPressure.dataMax_hex[2] =  moldPressure.i_maxval&0xFF;     
    analog_queue(&Qpressure,moldPressure.str);    

    
    analog_record(&inject_rs,"S",min_SPEED);// Inject ruler (Rs)RIS
    analog_record(&mold_rs,"s",min_SPEED);// mold ruler (Rs) RMS   
    analog_queue(&QrulerSpeed,mold_rs.str);
    
    analog_record(&inject_rp,"i",min_val);//RIm
    analog_record(&mold_rp,"m",min_val);//  RMm
    analog_queue(&QrulerMinPos,mold_rp.str_posMin); 
    
    analog_record(&inject_rp,"I",MAX_VAL);//RIM 
    analog_record(&mold_rp,"M",MAX_VAL);//RMM 
    analog_queue(&QrulerMaxPos,mold_rp.str);
    
   
    cycle_sync = false;
    Send_permite = true;    
}

if(Send_permite == true){
    
 // inject
    pvalidation(Qpressure.List[Qpressure.Index]); 
    pvalidation(injectPressure.str);    
    printf("injectPressure(max)=%d\n",injectPressure.i_maxval);
    CAN_Pub(NodeSub_SensorCard1_Data,injectPressure.dataMax_hex,HEX_LEN);   
    
 //mold close
    pvalidation(Qpressure.List[Qpressure.Index]);     
    pvalidation(moldPressure.str);    
    printf("moldPressure(max)=%d\n",moldPressure.i_maxval);
    CAN_Pub(NodeSub_SensorCard1_Data,moldPressure.dataMax_hex,HEX_LEN);
    
 //inject ruler speed    
    pvalidation(inject_rs.str);       
    printf("inject_rspeed(-max)=%d\n",inject_rs.i_minspeed);
    CAN_Pub(NodeSub_SensorCard1_Data,inject_rs.dataSpeedMin_hex,HEX_LEN);
    
 //mold ruler speed
    pvalidation(QrulerSpeed.List[QrulerSpeed.Index]);     
    pvalidation(mold_rs.str);       
    printf("mold_rspeed(-max)=%d\n",mold_rs.i_minspeed);
    CAN_Pub(NodeSub_SensorCard1_Data,mold_rs.dataSpeedMin_hex,HEX_LEN);   
    
 //inject ruler min position   
    pvalidation(inject_rp.str_posMin); 
    printf("inject_rulerPos(min)= %d\n",inject_rp.i_minval);    
    CAN_Pub(NodeSub_SensorCard1_Data,inject_rp.dataMin_hex,HEX_LEN);
    
 //inject ruler max position     
    pvalidation(inject_rp.str);       
    printf("inject_rulerPos(MAX)= %d\n",inject_rp.i_maxval);
    CAN_Pub(NodeSub_SensorCard1_Data,inject_rp.dataMax_hex,HEX_LEN);  
    
 //mold ruler min position   
    pvalidation(mold_rp.str_posMin);       
    printf("mold_rulerPos(min)= %d\n",mold_rp.i_minval);
    CAN_Pub(NodeSub_SensorCard1_Data,mold_rp.dataMin_hex,HEX_LEN);
     
 //mold ruler max position      
    pvalidation(mold_rp.str);       
    printf("mold_rulerPos(MAX)= %d\n",mold_rp.i_maxval);
    CAN_Pub(NodeSub_SensorCard1_Data,mold_rp.dataMax_hex,HEX_LEN);  


    analog_reset(&inject_rs);   
    analog_reset(&mold_rs);
    analog_reset(&inject_rp);   
    analog_reset(&mold_rp); 
    analog_reset(&pressure2[mold]);    
    analog_reset(&pressure1[mold]);
    analog_reset(&pressure2[inject]);    
    analog_reset(&pressure1[inject]);   
    analog_reset(&injectPressure);     
    analog_reset(&moldPressure);   

    Send_permite = false;    
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

#ifndef CALCULATE_FUN_
#define CALCULATE_FUN_
#include "stm32f1xx_hal.h"
#include "ads1115.h"
#include "math.h"
#include "stdio.h"
#include "canfunc.h"

#define Q_LEN 2
#define inject 0
#define mold 1  

extern Analog thermo;
extern Analog pressure[2];
extern Analog speed[2];
extern Analog inject_r;
extern Analog mold_r;

extern const float  offset_0;
extern const float offset_0_1; 
extern const float offset_0_5; 
extern const float offset_1_0; 
extern  const float offset_4_0; 


typedef struct{
    
volatile uint32_t t; // time
volatile uint32_t p; // period    
    
}Time;

typedef struct{
    
  char List[Q_LEN][A_LEN];
   uint8_t Index;    
    
}Queue;

typedef enum{
   BASE_500us = 2000,
   BASE_1ms = 1000    
    
}BASE_TIME;

typedef enum{
   MAX_VAL = 0u,
   VALUE,
   SPEED,
   MAX_SPEED,
   min_SPEED    
}VAL_opt;



extern Time time;

void adc_init(void);
float cal_speed(Analog *adc,BASE_TIME base);
void analog_record(Analog *adc,char *header,VAL_opt opt);
void analog_reset(Analog *adc);
void analog_queue(Queue *Q,char* str);
void pvalidation(char * str);
#endif
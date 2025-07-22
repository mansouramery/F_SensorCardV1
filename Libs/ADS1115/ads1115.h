#ifndef ADS1115_
#define ADS1115_
#include "main.h"



#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01
#define ADS1115_REG_LoThresh    0x02
#define ADS1115_REG_HighThresh  0x03

#define A_LEN 10

// Configuration options
typedef enum{
    ADS1115_ADD1 = (0x48 << 1), //GND connected
    ADS1115_ADD2 = (0x49 << 1)  // VCC connected  
       
}ADS1115_ADD;

typedef enum {
    ADS1115_MUX_AIN0_AIN1 = 0x0000,
    ADS1115_MUX_AIN0_AIN3 = 0x1000,
    ADS1115_MUX_AIN1_AIN3 = 0x2000,
    ADS1115_MUX_AIN2_AIN3 = 0x3000,
    ADS1115_MUX_AIN0_GND  = 0x4000,
    ADS1115_MUX_AIN1_GND  = 0x5000,
    ADS1115_MUX_AIN2_GND  = 0x6000,
    ADS1115_MUX_AIN3_GND  = 0x7000
} ADS1115_MUX;

typedef enum {
    ADS1115_PGA_6_144V = 0x0000,  // ±6.144V
    ADS1115_PGA_4_096V = 0x0200,   // ±4.096V
    ADS1115_PGA_2_048V = 0x0400,   // ±2.048V (default)
    ADS1115_PGA_1_024V = 0x0600,   // ±1.024V
    ADS1115_PGA_0_512V = 0x0800,   // ±0.512V
    ADS1115_PGA_0_256V = 0x0A00    // ±0.256V
} ADS1115_PGA;

typedef enum {
    ADS1115_MODE_CONTINUOUS = 0x0000,
    ADS1115_MODE_SINGLE     = 0x0100
} ADS1115_MODE;

typedef enum {
    ADS1115_DR_8SPS   = 0x0000,
    ADS1115_DR_16SPS  = 0x0020,
    ADS1115_DR_32SPS  = 0x0040,
    ADS1115_DR_64SPS  = 0x0060,
    ADS1115_DR_128SPS = 0x0080,  // default
    ADS1115_DR_250SPS = 0x00A0,
    ADS1115_DR_475SPS = 0x00C0,
    ADS1115_DR_860SPS = 0x00E0
} ADS1115_DR;

typedef struct{
 ADS1115_DR dr;
 ADS1115_PGA pga;
 ADS1115_MUX mux;   
 ADS1115_ADD addr;  
 int16_t adc_value;
 float f_value;
 float f_preval;
 uint16_t count;
 float f_maxval;
 int i_maxval;    
 int i_value;
 float pga_val;  
 float gain; 
 uint16_t rst_ct;    
 uint32_t Tick;
 uint32_t Tick_pre;
 uint32_t Tickp; 
 uint32_t Tickp_pre;      
 char str[A_LEN];  
 float speed;
 float prespeed;
 float maxspeed;
 float minspeed;
 int i_minspeed;
 float dispalacement;    
}Analog;
  
extern I2C_HandleTypeDef hi2c1;


HAL_StatusTypeDef ADS1115_WriteConfig(ADS1115_MUX mux, ADS1115_PGA pga, ADS1115_MODE mode, ADS1115_DR dr,ADS1115_ADD addr);
HAL_StatusTypeDef ADS1115_StartConversion(ADS1115_MUX mux,ADS1115_ADD addr,ADS1115_DR DR,ADS1115_PGA pga);
HAL_StatusTypeDef ADS1115_ReadConversion(int16_t *value,ADS1115_ADD addr);
HAL_StatusTypeDef ADS1115_ReadADC(Analog *adc_ch);
#endif



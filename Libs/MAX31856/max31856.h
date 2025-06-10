#ifndef MAX31856_
#define MAX31856_
#include "main.h"

extern SPI_HandleTypeDef hspi1;


#define MAX31856_CR0_REG      0x00
#define MAX31856_CR1_REG      0x01
#define MAX31856_MASK_REG     0x02
#define MAX31856_CJHF_REG     0x03
#define MAX31856_CJLF_REG     0x04
#define MAX31856_LTHFTH_REG   0x05
#define MAX31856_LTHFTL_REG   0x06
#define MAX31856_LTLFTH_REG   0x07
#define MAX31856_LTLFTL_REG   0x08
#define MAX31856_CJTO_REG     0x09
#define MAX31856_CJTH_REG     0x0A
#define MAX31856_CJTL_REG     0x0B
#define MAX31856_LTCBH_REG    0x0C
#define MAX31856_LTCBM_REG    0x0D
#define MAX31856_LTCBL_REG    0x0E
#define MAX31856_SR_REG       0x0F

// Thermocouple Types (CR1 Register)
typedef enum {
    MAX31856_TC_TYPE_B = 0,
    MAX31856_TC_TYPE_E = 1,
    MAX31856_TC_TYPE_J = 2,
    MAX31856_TC_TYPE_K = 3,
    MAX31856_TC_TYPE_N = 4,
    MAX31856_TC_TYPE_R = 5,
    MAX31856_TC_TYPE_S = 6,
    MAX31856_TC_TYPE_T = 7
} MAX31856_TC_Type;

// Averaging Modes (CR0 Register)
typedef enum {
    MAX31856_AVG_1   = 0x00,
    MAX31856_AVG_2   = 0x01,
    MAX31856_AVG_4   = 0x02,
    MAX31856_AVG_8   = 0x03,
    MAX31856_AVG_16  = 0x04
} MAX31856_AVG_Mode;

void MAX31856_WriteReg(uint8_t reg, uint8_t value, uint8_t CS);
uint8_t MAX31856_ReadReg(uint8_t reg,uint8_t CS);
void MAX31856_Init(MAX31856_TC_Type tcType, MAX31856_AVG_Mode avgMode,  uint8_t CS);
float MAX31856_ReadTemp(uint8_t CS);
float MAX31856_ReadCJTemp(uint8_t CS);
uint8_t MAX31856_ReadFault(uint8_t CS);
void MAX31856_TriggerOneShot(uint8_t CS) ;
#endif


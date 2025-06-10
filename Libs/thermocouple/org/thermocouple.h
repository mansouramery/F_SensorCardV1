#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include "stm32f1xx_hal.h"

typedef enum {
    K_TYPE,
    J_TYPE
} ThermocoupleType;

// Function prototypes
float thermocouple_read_temperature(ADC_HandleTypeDef *hadc, uint32_t adc_channel, 
                                  float cj_temp, ThermocoupleType type);
void thermocouple_init_adc(ADC_HandleTypeDef *hadc, uint32_t adc_channel);

#endif // THERMOCOUPLE_H
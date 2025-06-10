#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include "stm32f1xx_hal.h"

typedef enum {
    K_TYPE,
    J_TYPE
} ThermocoupleType;

// Function prototypes
float thermocouple_read_temperature(float thermocouple_voltage_mv,float cj_temp, ThermocoupleType type);
                                  
#endif // THERMOCOUPLE_H
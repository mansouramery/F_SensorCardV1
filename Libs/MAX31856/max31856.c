#include "max31856.h"


//.............................................................

void MAX31856_WriteReg(uint8_t reg, uint8_t value, uint8_t CS) {
    uint8_t txData[2] = {reg | 0x80, value}; // MSB=1 for write
    switch (CS){
        case 1:
            HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_RESET); 
            HAL_SPI_Transmit(&hspi1, txData, 2, 100);
            HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET);
            HAL_SPI_Transmit(&hspi1, txData, 2, 100);
            HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);         
        break;
        default:
          //.....
        break;            
    }

}

uint8_t MAX31856_ReadReg(uint8_t reg,uint8_t CS) {
    uint8_t txData[2] = {reg & 0x7F, 0x00}; // MSB=0 for read
    uint8_t rxData[2];
    switch (CS){
        case 1:
            HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_RESET); 
            HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, 100);
            HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, 100);
            HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);         
        break;
        default:
          //.....
        break;            
    }    
        
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS Low
//    HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 2, HAL_MAX_DELAY);
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // CS High
    return rxData[1];
}

void MAX31856_Init(MAX31856_TC_Type tcType, MAX31856_AVG_Mode avgMode,  uint8_t CS) {
    // Configure CR0 (50Hz noise rejection + averaging)
    MAX31856_WriteReg(MAX31856_CR0_REG, 0x03 | (avgMode << 4),CS);

    // Configure CR1 (Thermocouple type + cold-junction enable)
    MAX31856_WriteReg(MAX31856_CR1_REG, (tcType << 4) | 0x01,CS);
}

float MAX31856_ReadTemp(uint8_t CS) {
    uint8_t buffer[3];
    int32_t rawTemp;

    // Read 3 bytes (19-bit thermocouple temp)
    buffer[0] = MAX31856_ReadReg(MAX31856_LTCBH_REG,CS);
    buffer[1] = MAX31856_ReadReg(MAX31856_LTCBM_REG,CS);
    buffer[2] = MAX31856_ReadReg(MAX31856_LTCBL_REG,CS);

    // Combine into 19-bit signed value
    rawTemp = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
    rawTemp >>= 5; // Right-align 19-bit value

    // Convert to float (°C)
    return (float)rawTemp * 0.0078125f; // LSB = 0.0078125°C
}

float MAX31856_ReadCJTemp(uint8_t CS) {
    uint8_t buffer[2];
    int16_t rawCJTemp;

    buffer[0] = MAX31856_ReadReg(MAX31856_CJTH_REG,CS);
    buffer[1] = MAX31856_ReadReg(MAX31856_CJTL_REG,CS);

    rawCJTemp = (buffer[0] << 8) | buffer[1];
    return (float)rawCJTemp * 0.015625f; // LSB = 0.015625°C
}


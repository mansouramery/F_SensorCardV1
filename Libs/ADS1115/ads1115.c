#include "ads1115.h"



//.................................................................


HAL_StatusTypeDef ADS1115_WriteConfig(ADS1115_MUX mux, ADS1115_PGA pga, ADS1115_MODE mode, ADS1115_DR dr,ADS1115_ADD addr)
{
    uint8_t config[3];
    uint16_t config_value = 0x8000 | mux | pga | mode | dr | 0x0003; // 0x0003 = disable comparator
    
    config[0] = ADS1115_REG_CONFIG;
    config[1] = (config_value >> 8) & 0xFF;  // MSB first
    config[2] = config_value & 0xFF;         // LSB
    
    return HAL_I2C_Master_Transmit(&hi2c1, addr, config, 3, 100);
}


HAL_StatusTypeDef ADS1115_StartConversion(ADS1115_MUX mux,ADS1115_ADD addr, ADS1115_DR DR,ADS1115_PGA pga)
{
    uint8_t config[3];
    uint16_t config_value = 0x8000 | mux | pga | ADS1115_MODE_SINGLE | DR | 0x0003;
    //uint16_t config_value = 0x8000 | mux | ADS1115_PGA_6_144V | ADS1115_MODE_SINGLE | ADS1115_DR_128SPS | 0x0003;
    
    config[0] = ADS1115_REG_CONFIG;
    config[1] = (config_value >> 8) & 0xFF;
    config[2] = config_value & 0xFF;
    
    return HAL_I2C_Master_Transmit(&hi2c1, addr, config, 3, 100);
}


HAL_StatusTypeDef ADS1115_ReadConversion(int16_t *value,ADS1115_ADD addr)
{
    uint8_t reg = ADS1115_REG_CONVERSION;
    uint8_t data[2];
    HAL_StatusTypeDef status;
    
    // Point to conversion register
    status = HAL_I2C_Master_Transmit(&hi2c1, addr, &reg, 1, 100);
    if (status != HAL_OK) return status;
    
    // Read conversion result
    status = HAL_I2C_Master_Receive(&hi2c1, addr, data, 2, 100);
    if (status == HAL_OK) {
        *value = (int16_t)((data[0] << 8) | data[1]);
    }
    
    return status;
}


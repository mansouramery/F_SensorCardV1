#include "ads1115.h"
#include "math.h"
#include "stdio.h"


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

HAL_StatusTypeDef ADS1115_ReadADC(ADS1115_CH *adc_ch){  
    if(adc_ch->pga_val == 0.0){
    switch (adc_ch->pga) {
        case ADS1115_PGA_6_144V:
          adc_ch->pga_val = 6.144f;
        break;
        case ADS1115_PGA_4_096V:
          adc_ch->pga_val = 4.096f;
        break;
        case ADS1115_PGA_2_048V:
          adc_ch->pga_val = 2.048f;
        break;
        case ADS1115_PGA_1_024V:
          adc_ch->pga_val = 1.024f;
        break;
        case ADS1115_PGA_0_512V:
          adc_ch->pga_val = 0.512f;
        break;
        case ADS1115_PGA_0_256V:
          adc_ch->pga_val = 0.256f;
        break;
        default:
          adc_ch->pga_val = 2.048f; 
        break;
        
    }
}
    
        if (ADS1115_StartConversion(adc_ch->mux,adc_ch->addr,adc_ch->dr,adc_ch->pga) == HAL_OK)
        {
            HAL_Delay(1);  // Wait for conversion (depends on data rate)
            
            if (ADS1115_ReadConversion(&adc_ch->adc_value,adc_ch->addr) == HAL_OK)
            {
                // Convert to voltage (depends on PGA setting)
                adc_ch->f_value = (adc_ch->adc_value * adc_ch->pga_val) / 32768.0f;
                adc_ch->i_value = round(1000*(adc_ch->gain)*(adc_ch->f_value));
                //printf("I_VALUE=%d,GAIN=%f. F_VAL=%f\n",adc_ch->i_value,adc_ch->gain,adc_ch->f_value);
                if((adc_ch->f_value) > (adc_ch->f_preval)){
                adc_ch->i_maxval = adc_ch->i_value;
                    adc_ch->f_preval = adc_ch->f_value;
                }else{
                    adc_ch->i_maxval = round(1000*adc_ch->gain*(adc_ch->f_preval));
                    adc_ch->count++;
                    //HAL_GPIO_TogglePin(LED4_GPIO_Port,LED4_Pin);                     
                }
                if(adc_ch->count >adc_ch->rst_ct){
                   adc_ch->count = 0; 
                    adc_ch->f_preval = 0;
                }
                    
            }
        } 
        else{
         printf("ADC EROOR\n");
        }            
    
    
    //printf("\nbbbbbbbb\n");
}



/*


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
        
*/

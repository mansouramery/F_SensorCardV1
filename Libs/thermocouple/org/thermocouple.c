#include "thermocouple.h"
#include "math.h"

// NIST ITS-90 coefficients for K-type thermocouple (0°C to 500°C, voltage in mV)
static const float k_type_coeff[] = {
    0.0,                        // c0
    2.508355e1,                 // c1
    7.860106e-2,                // c2
    -2.503131e-1,               // c3
    8.315270e-2,                // c4
    -1.228618e-2,               // c5
    9.804036e-4,                // c6
    -4.413030e-5,               // c7
    1.057734e-6,                // c8
    -1.052755e-8                // c9
};

// NIST ITS-90 coefficients for J-type thermocouple (0°C to 760°C, voltage in mV)
static const float j_type_coeff[] = {
    0.0,                        // c0
    1.978425e1,                 // c1
    -2.001204e-1,               // c2
    1.036969e-2,                // c3
    -2.549687e-4,               // c4
    3.585153e-6,                // c5
    -3.113581e-8,               // c6
    1.320904e-10,               // c7
    -2.279465e-13               // c8
};

// ADC reference voltage (3.3V typical for STM32)
#define ADC_VREF_MV 3300.0f
#define ADC_sMAX_VALUE 4095.0f // 12-bit ADC

// Thermocouple voltage-to-temperature conversion
static float thermocouple_voltage_to_temp(float voltage_mv, ThermocoupleType type) {
    const float *coeff = (type == K_TYPE) ? k_type_coeff : j_type_coeff;
    int coeff_count = (type == K_TYPE) ? 10 : 9;

    float temp = 0.0f;
    for (int i = 0; i < coeff_count; i++) {
        temp += coeff[i] * powf(voltage_mv, i);
    }
    return temp;
}

// Convert cold junction temperature to equivalent thermocouple voltage
static float cold_junction_to_voltage(float cj_temp, ThermocoupleType type) {
    // Simplified linear approximation for cold junction (valid for 0°C to 50°C)
    float seebeck_coeff = (type == K_TYPE) ? 0.041f : 0.052f; // mV/°C for K-type, J-type
    return seebeck_coeff * cj_temp;
}

// Main function to convert ADC and cold junction to temperature
float thermocouple_read_temperature(ADC_HandleTypeDef *hadc, uint32_t adc_channel, 
                                  float cj_temp, ThermocoupleType type) {
    // Start ADC conversion
    HAL_ADC_Start(hadc);
    if (HAL_ADC_PollForConversion(hadc, 100) != HAL_OK) {
        return -999.0f; // Error
    }

    // Read ADC value
    uint32_t adc_value = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);

    // Convert ADC to voltage (mV)
    float adc_voltage_mv = (adc_value * ADC_VREF_MV) / ADC_MAX_VALUE;

    // Convert ADC voltage to thermocouple voltage
    // Assume thermocouple is connected to a differential amplifier with gain=100
    float thermocouple_voltage_mv = adc_voltage_mv / 100.0f;

    // Add cold junction compensation
    float cj_voltage_mv = cold_junction_to_voltage(cj_temp, type);
    float total_voltage_mv = thermocouple_voltage_mv + cj_voltage_mv;

    // Convert total voltage to temperature
    return thermocouple_voltage_to_temp(total_voltage_mv, type);
}

// Initialize ADC for thermocouple measurement
void thermocouple_init_adc(ADC_HandleTypeDef *hadc, uint32_t adc_channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // Configure ADC channel
    sConfig.Channel = adc_channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
        // Handle error
        while (1);
    }

    // Calibrate ADC
    if (HAL_ADCEx_Calibration_Start(hadc, ADC_SINGLE_ENDED) != HAL_OK) {
        // Handle error
        while (1);
    }
}
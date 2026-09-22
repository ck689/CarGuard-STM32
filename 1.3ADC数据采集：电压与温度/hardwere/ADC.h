#ifndef _ADC_H
#define _ADC_H
void ADC1_Init(void) ;
uint16_t ADC_GetValue(uint8_t channel) ;
float Voltage_Get(void);
float NTC_GetTemp(void);
float Sample_Avg(uint8_t channel);
#endif

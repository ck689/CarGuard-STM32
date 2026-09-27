#ifndef _ADC_H
#define _ADC_H

/**
 * @brief  ADC1 初始化（PA0=电压, PA1=NTC温度，单次转换+软件触发）
 * @param  无
 * @retval 无
 */
void ADC1_Init(void);

/**
 * @brief  读取指定ADC通道的单次转换值（阻塞等待）
 * @param  channel ADC通道号（ADC_Channel_0~15）
 * @retval 12位ADC结果，范围0~4095
 */
uint16_t ADC_GetValue(uint8_t channel);

/**
 * @brief  读取电压（PA0，单次采样，分压比2）
 * @param  无
 * @retval 实际电压值（V），约0~6.6V
 */
float Voltage_Get(void);

/**
 * @brief  读取NTC温度（PA1，B值公式，R0=10K,B=3950）
 * @param  无
 * @retval 摄氏温度值（℃）
 */
float NTC_GetTemp(void);

/**
 * @brief  指定通道10次采样取平均值（软件滤波）
 * @param  channel ADC通道号
 * @retval 平均ADC原始值（浮点），范围0~4095
 */
float Sample_Avg(uint8_t channel);

#endif

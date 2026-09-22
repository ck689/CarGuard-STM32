#include "stm32f10x.h"                  // Device header
#include<math.h>
void ADC1_Init(void) 
	{
		GPIO_InitTypeDef GPIO_InitStructure; 
		ADC_InitTypeDef ADC_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
		RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADC时钟 = 72M/6 = 12MHz（必须≤14MHz）
	
	
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;  // PA0电压 + PA1温度
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;// 单次转换（非连续）
		ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right; // 右对齐（数值直接用）
		ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None; // 软件触发
		ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;// 独立模式（不用双ADC同步）
		ADC_InitStructure.ADC_NbrOfChannel=1; // 1个通道
		ADC_InitStructure.ADC_ScanConvMode=DISABLE;// 非扫描（单通道）
		
		ADC_Init(ADC1, &ADC_InitStructure);

		// 注意：规则通道配置不在初始化里写死，改到 ADC_GetValue 里每次读数前配置
		ADC_Cmd(ADC1, ENABLE); // 使能 ADC 外设
		
		ADC_ResetCalibration(ADC1); // 复位校准 
		while(ADC_GetResetCalibrationStatus(ADC1)); // 等复位完成 
		ADC_StartCalibration(ADC1); // 开始校准
		while(ADC_GetCalibrationStatus(ADC1)); // 等校准完成 
		}
	
	
	uint16_t ADC_GetValue(uint8_t channel)
			{
				ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5); // 每次读数前配置通道
				ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 软件触发，开始转换
				while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // 等转换完成（EOC=转换结束）
				return ADC_GetConversionValue(ADC1); // 读取转换结果（0~4095）
				}
			
	float Voltage_Get(void) 
		{ 
			uint16_t adc = ADC_GetValue(ADC_Channel_0);  // 测 PA0（电压）
			float v_adc = adc * 3.3f / 4096.0f; // ADC值 → 分压点电压（0~3.3V）
			float v_real = v_adc * 2.0f; // ×分压比 = 实际电压（0~6.6V） 
			return v_real; 
		}
		
		float NTC_GetTemp(void)
			{
				// PA1 接的是 NTC 和 10K 上拉电阻的分压点 
				// 接线：3V3 → NTC → PA1 → 10K电阻 → GND 
				uint16_t adc = ADC_GetValue(ADC_Channel_1);  // 测 PA1（NTC温度）
				float v = adc * 3.3f / 4096.0f; // PA1 电压 // 由分压公式反推 NTC 阻值： // v = 3.3 × R_fixed / (R_ntc + R_fixed) // → R_ntc = R_fixed × (3.3/v - 1) 
				float r_ntc = 10000.0f * (3.3f / v - 1.0f); // B 值公式：T = 1 / (1/T0 + ln(R/R0)/B) - 273.15 // T0=298.15K(25℃), R0=10000Ω, B=3950 
				float inv_t = 1.0f/298.15f + logf(r_ntc/10000.0f)/3950.0f; 
				float temp_k = 1.0f / inv_t; // 开尔文温度 
				float temp_c = temp_k - 273.15f; // 转摄氏温度 
				return temp_c; 
			}
			
	float Sample_Avg(uint8_t channel) {
		uint32_t sum = 0; 
		for (uint8_t i = 0; i < 10; i++) {
			sum += ADC_GetValue(channel);
			} return sum / 10.0f; // 返回平均 ADC 值，再拿去换算电压/温度 
		}
	
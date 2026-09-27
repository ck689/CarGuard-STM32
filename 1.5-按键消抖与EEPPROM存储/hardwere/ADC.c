#include "stm32f10x.h"
#include <math.h>

/**
 * @brief  ADC1 初始化（PA0=电压采集, PA1=NTC温度采集）
 * @note   配置为单次转换、软件触发、右对齐、独立模式
 *         ADC时钟=72MHz/6=12MHz（必须≤14MHz）
 *         初始化后执行复位校准+开始校准，确保采集精度
 * @param  无
 * @retval 无
 */
void ADC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   /* ADC时钟 = 72M/6 = 12MHz（必须≤14MHz） */

	/* PA0、PA1 配置为模拟输入模式（ADC采集引脚必须为模拟输入） */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;       /* 单次转换（非连续） */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;     /* 右对齐（数值可直接使用） */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  /* 软件触发 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;         /* 独立模式（不使用双ADC同步） */
	ADC_InitStructure.ADC_NbrOfChannel = 1;                     /* 1个通道 */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;               /* 非扫描（单通道） */
	ADC_Init(ADC1, &ADC_InitStructure);

	/* 规则通道配置不在初始化里写死，改到 ADC_GetValue 里每次读数前配置 */
	ADC_Cmd(ADC1, ENABLE);                    /* 使能 ADC 外设 */

	ADC_ResetCalibration(ADC1);               /* 复位校准 */
	while (ADC_GetResetCalibrationStatus(ADC1));  /* 等待复位完成 */
	ADC_StartCalibration(ADC1);               /* 开始校准 */
	while (ADC_GetCalibrationStatus(ADC1));   /* 等待校准完成 */
}

/**
 * @brief  读取指定ADC通道的单次转换值（阻塞式，等待转换完成）
 * @note   每次读数前重新配置规则通道，支持多通道轮流采集
 *         采样时间设为239.5周期，提高采集稳定性
 * @param  channel ADC通道号，如 ADC_Channel_0（PA0）、ADC_Channel_1（PA1）
 * @retval 12位ADC转换结果，范围：0~4095
 */
uint16_t ADC_GetValue(uint8_t channel)
{
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);  /* 配置通道+采样时间 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);    /* 软件触发，开始转换 */
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);  /* 等待转换完成（EOC=转换结束标志） */
	return ADC_GetConversionValue(ADC1);        /* 读取转换结果（0~4095） */
}

/**
 * @brief  读取电压（PA0，单次采样，不含平均滤波）
 * @note   硬件分压比=2（实际电压经2:1分压后接入ADC）
 *         换算公式：实际电压 = ADC值 × 3.3 / 4096 × 2
 * @param  无
 * @retval 实际电压值，单位V，范围约0~6.6V
 */
float Voltage_Get(void)
{
	uint16_t adc = ADC_GetValue(ADC_Channel_0);          /* 测 PA0（电压） */
	float v_adc = adc * 3.3f / 4096.0f;                  /* ADC值 → 分压点电压（0~3.3V） */
	float v_real = v_adc * 2.0f;                           /* ×分压比 = 实际电压（0~6.6V） */
	return v_real;
}

/**
 * @brief  读取NTC温度（PA1，B值公式换算）
 * @note   硬件接线：3V3 → NTC → PA1 → 10K固定电阻 → GND
 *         先由分压公式反推NTC阻值，再用B值公式换算温度
 *         NTC参数：R0=10KΩ(25℃), B=3950, T0=298.15K
 * @param  无
 * @retval 摄氏温度值，单位℃
 */
float NTC_GetTemp(void)
{
	/* PA1 接的是 NTC 和 10K 上拉电阻的分压点
	   接线：3V3 → NTC → PA1 → 10K电阻 → GND */
	uint16_t adc = ADC_GetValue(ADC_Channel_1);           /* 测 PA1（NTC温度） */
	float v = adc * 3.3f / 4096.0f;                        /* PA1 电压 */

	/* 由分压公式反推 NTC 阻值：
	   v = 3.3 × R_fixed / (R_ntc + R_fixed)
	   → R_ntc = R_fixed × (3.3/v - 1) */
	float r_ntc = 10000.0f * (3.3f / v - 1.0f);

	/* B 值公式：T = 1 / (1/T0 + ln(R/R0)/B) - 273.15
	   T0=298.15K(25℃), R0=10000Ω, B=3950 */
	float inv_t = 1.0f / 298.15f + logf(r_ntc / 10000.0f) / 3950.0f;
	float temp_k = 1.0f / inv_t;     /* 开尔文温度 */
	float temp_c = temp_k - 273.15f;  /* 转摄氏温度 */
	return temp_c;
}

/**
 * @brief  指定通道多次采样取平均值（软件滤波，减小随机噪声）
 * @note   连续采样10次取算术平均，返回平均ADC原始值
 *         调用方需自行将ADC值换算为电压或温度
 * @param  channel ADC通道号
 * @retval 10次采样的平均ADC值（浮点），范围0~4095
 */
float Sample_Avg(uint8_t channel)
{
	uint32_t sum = 0;
	for (uint8_t i = 0; i < 10; i++)
	{
		sum += ADC_GetValue(channel);
	}
	return sum / 10.0f;   /* 返回平均 ADC 值，再拿去换算电压/温度 */
}

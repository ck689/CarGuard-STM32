#include "stm32f10x.h"
#include "Delay.h"      /* 延时函数：Delay_ms/Delay_us */
#include "LED.h"        /* LED 驱动：LED_Init/LED_Set/LED_Get（PA8） */
#include "BEEP.h"       /* 蜂鸣器驱动：BEEP_Init/BEEP_Set（PB0） */
#include "USART.h"      /* 串口驱动：USART1_Init，含printf重定向（PA9/PA10） */
#include "ADC.h"        /* V1：ADC电压温度采集（PA0=电压, PA1=NTC温度） */
#include "OLED.h"       /* V2：OLED显示（软件I2C，PB8=SCL, PB9=SDA） */
#include <string.h>
#include <stdio.h>

/**
 * @brief  主函数：车载智能监测终端 V2
 * @note   V2 在 V1（ADC采集+串口打印）基础上增加 OLED 显示
 *         采用局部刷新策略：静态标签只写一次，动态数据只更新数字区域，避免整屏闪烁
 * @retval int（不会返回，死循环）
 */
int main(void)
{
	/* ===== 外设初始化（只执行一次）===== */
	LED_Init();              /* PA8 推挽输出，状态指示灯 */
	BEEP_Init();             /* PB0 推挽输出，蜂鸣器 */
	USART1_Init(115200);    /* 串口1初始化，波特率115200 */
	ADC1_Init();             /* V1：ADC初始化，PA0电压+PA1温度 */
	OLED_Init();             /* V2：OLED初始化，软件I2C */

	printf("CarGuard V2 启动成功 | ADC采集 + OLED显示\r\n");

	/* ===== V2：静态标签只写一次（局部刷新核心，避免闪烁）===== */
	OLED_ShowString(1, 1, "Volt:");      /* 第1行：电压标签 */
	OLED_ShowString(2, 1, "Temp:");      /* 第2行：温度标签 */
	OLED_ShowString(3, 1, "Status:");    /* 第3行：状态标签 */
	OLED_ShowString(4, 1, "CarGuard V2");/* 第4行：版本信息 */

	/* ===== 主循环（死循环）===== */
	while (1)
	{
		/* ① 采集电压：PA0，10次平均采样后换算实际电压
		   分压比=2，ADC参考电压3.3V，12位分辨率(0~4095) */
		float volt = Sample_Avg(ADC_Channel_0) * 3.3f / 4096.0f * 2.0f;

		/* ② 采集温度：PA1，NTC热敏电阻分压+B值公式换算 */
		float temp = NTC_GetTemp();

		/* ③ 串口打印（调试用，与OLED数值对照） */
		printf("电压: %.2fV | 温度: %.1f℃\r\n", volt, temp);

		/* ④ V2：OLED局部刷新——只更新数字区域，不整屏清屏
		   电压：第1行第6列起，2位整数+2位小数（含符号共6列），单位V在第12列 */
		OLED_ShowFloatNum(1, 6, volt, 2, 2);
		OLED_ShowChar(1, 12, 'V');

		/* 温度：第2行第6列起，2位整数+1位小数（含符号共5列），单位C在第11列 */
		OLED_ShowFloatNum(2, 6, temp, 2, 1);
		OLED_ShowChar(2, 11, 'C');

		/* ⑤ 报警逻辑：电压超标时LED亮+OLED显示警告，否则LED灭+显示正常 */
		if (volt > 14.0f)
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);  /* LED点亮（低电平有效） */
			OLED_ShowString(3, 8, "Warn!!");     /* 状态：警告 */
			printf("[警告] 电压过高!\r\n");
		}
		else
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_8);     /* LED熄灭 */
			OLED_ShowString(3, 8, "Normal");      /* 状态：正常 */
		}

		Delay_ms(500);   /* 500ms采集刷新一次（V2阶段用Delay，V4将改为定时器调度） */
	}
}

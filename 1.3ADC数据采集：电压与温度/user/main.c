#include "stm32f10x.h"
#include "Delay.h" 
#include "LED.h"      // LED 驱动（PA8 推挽输出）
#include "BEEP.h"     // 蜂鸣器驱动（PB0 推挽输出）
#include "USART.h"    // 串口驱动（含 printf 重定向）
#include "ADC.h"      // V1：ADC 电压温度采集
#include "OLED.h"     // V2：OLED 显示（软件 I2C，PB8=SCL, PB9=SDA）
#include <string.h>
#include <stdio.h>

int main(void) 
{
	// ===== 初始化（只执行一次）===== 
	LED_Init();        // PA8 推挽输出
	BEEP_Init();       // PB0 推挽输出 
	USART1_Init(115200); // 串口 115200
	ADC1_Init();       // V1：ADC 初始化（PA0=电压, PA1=NTC温度）
	OLED_Init();       // V2：OLED 初始化（软件 I2C）

	printf("CarGuard V2 启动成功 | ADC采集 + OLED显示\r\n");

	// ===== V2：静态标签只写一次（局部刷新，避免闪烁）=====
	OLED_ShowString(1, 1, "Volt:");    // 第1行：电压标签
	OLED_ShowString(2, 1, "Temp:");    // 第2行：温度标签
	OLED_ShowString(3, 1, "Status:");  // 第3行：状态标签
	OLED_ShowString(4, 1, "CarGuard V2"); // 第4行：版本信息

	// ===== 主循环（死循环）=====
	while(1)
	{
		// ① 读电压（PA0，10次平均）
		float volt = Sample_Avg(ADC_Channel_0) * 3.3f / 4096.0f * 2.0f;

		// ② 读温度（PA1，NTC）
		float temp = NTC_GetTemp();

		// ③ 串口打印
		printf("电压: %.2fV | 温度: %.1f℃\r\n", volt, temp);

		// ④ V2：OLED 局部刷新（只更新数字区域，不整屏清屏）
		//    电压：第1行第6列起，2位整数+2位小数（含符号共6列），单位V在第12列
		OLED_ShowFloatNum(1, 6, volt, 2, 2);
		OLED_ShowChar(1, 12, 'V');

		//    温度：第2行第6列起，2位整数+1位小数（含符号共5列），单位C在第11列
		OLED_ShowFloatNum(2, 6, temp, 2, 1);
		OLED_ShowChar(2, 11, 'C');

		// ⑤ 报警逻辑（超压闪灯 + OLED状态显示）
		if(volt > 14.0f)
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);  // LED亮（低电平点亮）
			OLED_ShowString(3, 8, "Warn!!");     // 状态：警告
			printf("[警告] 电压过高!\r\n");
		}
		else
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_8);     // LED灭
			OLED_ShowString(3, 8, "Normal");      // 状态：正常
		}

		Delay_ms(500);   // 500ms 采一次（V2 阶段还用 Delay，V4 会改成定时器）
	}
}

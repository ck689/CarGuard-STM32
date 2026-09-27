#include "stm32f10x.h"
#include "Delay.h"      /* 延时函数：Delay_ms/Delay_us */
#include "LED.h"        /* LED 驱动：LED_Init/LED_Set（PA8） */
#include "BEEP.h"       /* 蜂鸣器驱动：BEEP_Init/BEEP_Set（PB0） */
#include "USART.h"      /* 串口驱动：USART1_Init，含printf重定向（PA9/PA10） */
#include "ADC.h"        /* V1：ADC电压温度采集（PA0=电压, PA1=NTC温度） */
#include "OLED.h"       /* V2：OLED显示（软件I2C，PB8=SCL, PB9=SDA） */
#include "key.h"        /* V3：按键输入（PB4=KEY1, PB5=KEY2） */
#include "AT24C02.h"    /* V3：EEPROM掉电存储 + 参数加载/保存 */
#include <string.h>
#include <stdio.h>

/**
 * @brief  主函数：车载智能监测终端 V3
 * @note   V3 在 V2（ADC采集+OLED显示）基础上增加：
 *           - 按键输入（PB4/PB5，状态消抖）
 *           - AT24C02 EEPROM 掉电存储
 *           - 按键调节电压阈值，调节后自动保存
 *           - 上电从 EEPROM 加载阈值（首次上电写默认值）
 *         OLED布局：第1行电压，第2行温度，第3行状态，第4行阈值
 * @retval int（不会返回，死循环）
 */
int main(void)
{
	/* ===== 外设初始化（只执行一次）===== */
	LED_Init();              /* PA8 推挽输出，报警指示灯 */
	BEEP_Init();             /* PB0 推挽输出，蜂鸣器 */
	USART1_Init(115200);     /* 串口1初始化，波特率115200 */
	ADC1_Init();             /* V1：ADC初始化，PA0电压+PA1温度 */
	OLED_Init();             /* V2：OLED初始化，软件I2C（PB8/PB9） */
	Key_Init();              /* V3：按键初始化（PB4/PB5，内部上拉） */
	Params_Load();           /* V3：从EEPROM加载阈值（首次上电写默认值） */

	printf("CarGuard V3 启动成功 | 按键调阈值 + EEPROM掉电保存\r\n");
	printf("当前阈值: 电压=%.1fV 温度=%dC\r\n", volt_th / 10.0f, temp_th);

	/* ===== 静态标签只写一次（局部刷新，避免整屏闪烁）===== */
	OLED_ShowString(1, 1, "电压:");
	OLED_ShowString(2, 1, "温度:");
	OLED_ShowString(3, 1, "状态:");
	OLED_ShowString(4, 1, "阈值:");      /* V3：第4行显示当前阈值 */

	/* ===== 主循环（死循环）===== */
	while (1)
	{
		/* ① 采集电压：PA0，10次平均采样后换算实际电压
		   分压比=2，ADC参考电压3.3V，12位分辨率(0~4095) */
		float volt = Sample_Avg(ADC_Channel_0) * 3.3f / 4096.0f * 2.0f;

		/* ② 采集温度：PA1，NTC热敏电阻分压+B值公式换算 */
		float temp = NTC_GetTemp();

		/* ③ V3：按键扫描——按键1=阈值+0.5V，按键2=阈值-0.5V */
		uint8_t key = Key_Scan();
		if (key == KEY1_PRESS)
		{
			volt_th += 5;                    /* +0.5V（volt_th单位是0.1V） */
			if (volt_th > 200) volt_th = 200; /* 上限 20.0V */
			Params_Save();                   /* 调节后保存到EEPROM */
			printf("[KEY] 阈值+  当前=%.1fV\r\n", volt_th / 10.0f);
		}
		else if (key == KEY2_PRESS)
		{
			if (volt_th > 50) volt_th -= 5;   /* -0.5V，下限 5.0V */
			else              volt_th = 50;
			Params_Save();
			printf("[KEY] 阈值-  当前=%.1fV\r\n", volt_th / 10.0f);
		}

		/* ④ 串口打印（调试用） */
		printf("电压: %.2fV | 温度: %.1fC | 阈值: %.1fV\r\n", volt, temp, volt_th / 10.0f);

		/* ⑤ OLED局部刷新——只更新数字区域，不整屏清屏 */
		OLED_ShowFloatNum(1, 6, volt, 2, 2);
		OLED_ShowChar(1, 12, 'V');

		OLED_ShowFloatNum(2, 6, temp, 2, 1);
		OLED_ShowChar(2, 11, 'C');

		/* V3：第4行刷新阈值（2位整数+1位小数，单位V） */
		OLED_ShowFloatNum(4, 6, volt_th / 10.0f, 2, 1);
		OLED_ShowChar(4, 11, 'V');

		/* ⑥ 报警逻辑：电压超过 EEPROM 保存的阈值 → LED亮+OLED警告，否则正常 */
		if (volt > volt_th / 10.0f)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_8);    /* LED点亮 */
			BEEP_Set(1);                         /* 蜂鸣器鸣叫（低电平触发） */
			OLED_ShowString(3, 8, "Warn!!");
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);  /* LED熄灭 */
			BEEP_Set(0);                         /* 蜂鸣器停止 */
			OLED_ShowString(3, 8, "Normal");
		}

		Delay_ms(30);    /* V3阶段用Delay，30ms≈33Hz扫描，按键更灵敏；V4改定时器非阻塞 */
	}
}

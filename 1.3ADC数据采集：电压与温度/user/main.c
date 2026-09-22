#include "stm32f10x.h"
#include "Delay.h" 
#include "LED.h" // LED 驱动（自己写：LED_Init/LED_Set/LED_Get）
#include "BEEP.h" // 蜂鸣器驱动
#include "USART.h" // 串口驱动（含 RxBuf/RxFlag） 
#include <string.h> // strcmp 所需
#include <stdio.h> // printf 所需 
#include "LED.h"
#include "ADC.h"

int main(void) 
	{
		// ===== 初始化（只执行一次）===== 
	LED_Init(); // PA8 推挽输出
	BEEP_Init(); // PB0 推挽输出 
	USART1_Init(115200); // 串口 115200
    ADC1_Init();            // V1：ADC 初始化（新增）

    printf("CarGuard V1 启动成功 | ADC 电压温度采集\r\n");

    // ===== 主循环（死循环）=====
    while(1)
    {
        // ① 读电压（PA0，10次平均）
        float volt = Sample_Avg(ADC_Channel_0) * 3.3f / 4096.0f * 2.0f;

        // ② 读温度（PA1，NTC）
        float temp = NTC_GetTemp();

        // ③ 串口打印
        printf("电压: %.2fV | 温度: %.1f℃\r\n", volt, temp);

        // ④ 简单报警逻辑（超压闪灯）
        if(volt > 14.0f) {
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);
            printf("[警告] 电压过高!\r\n");
        } else {
            GPIO_SetBits(GPIOA,GPIO_Pin_8);
        }

        Delay_ms(500);   // 500ms 采一次（V1 阶段还用 Delay，V4 会改掉）
    }
}
	
	
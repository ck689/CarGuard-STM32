#include "stm32f10x.h"
#include "Delay.h" 
#include "LED.h" // LED 驱动（自己写：LED_Init/LED_Set/LED_Get）
#include "BEEP.h" // 蜂鸣器驱动
#include "USART.h" // 串口驱动（含 RxBuf/RxFlag） 
#include <string.h> // strcmp 所需
#include <stdio.h> // printf 所需 
#include "LED.h"


int main(void) 
	{
		// ===== 初始化（只执行一次）===== 
	LED_Init(); // PA8 推挽输出
	BEEP_Init(); // PB0 推挽输出 
	USART1_Init(115200); // 串口 115200
	printf("================================\r\n"); 
	printf(" CarGuard V0 启动成功！\r\n");
	printf(" 支持指令: LED1_ON / LED1_OFF\r\n"); 
	printf(" BEEP_ON / BEEP_OFF\r\n");
	printf(" STATUS\r\n");
	printf("================================\r\n"); // ===== 主循环（死循环）===== 
	while (1) 
		{ 
	if (RxFlag == 1) // 有新指令到达 
	{ 
		RxFlag = 0; // 清标志位
		// ===== 指令解析（用 strcmp 比较字符串）===== 
		if (strcmp((char*)RxBuf, "LED1_ON") == 0) 
			{ 
				LED_Set(1);
				printf("[OK] LED 已点亮\r\n");
				}
				else if (strcmp((char*)RxBuf, "LED1_OFF") == 0) 
					{ 
						LED_Set(0);
						printf("[OK] LED 已熄灭\r\n");
						} 
						else if (strcmp((char*)RxBuf, "BEEP_ON") == 0)
							{ 
								BEEP_Set(1); printf("[OK] 蜂鸣器已开启\r\n");
								}
								else if (strcmp((char*)RxBuf, "BEEP_OFF") == 0)
									{ 
										BEEP_Set(0); printf("[OK] 蜂鸣器已关闭\r\n");
										}
										else if (strcmp((char*)RxBuf, "STATUS") == 0)
											{ 
												printf("[状态] LED:%s | BEEP:%s\r\n", LED_Get() ? "ON" : "OFF", BEEP_Get() ? "ON" : "OFF"); 
												}
												else 
													{ printf("[错误] 未知指令: %s\r\n", RxBuf);
														} 
													RxLen = 0; // 清空接收长度，准备下一帧 
														} 
}
}

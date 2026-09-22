#include "stm32f10x.h"                  // Device header
#include"Delay.h"
#include"OLED.h"
#include"Serial.h"
#include <stdio.h>
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t) ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
int main(void)
{	
	OLED_Init();
	
	Serial_Init();
	printf("CarGuard 环境验证成功！\r\n");
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	while(1)
		
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_13); // 亮（低电平点亮，蓝板LED是低电平亮）
		Delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_13); // 灭
		Delay_ms(500); 
		printf("系统运行中... LED 闪烁正常\r\n");
		
	}
	
}

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
uint8_t RxBuf[64]; // 接收缓冲区，最多存63个字符 
uint8_t RxLen = 0; // 当前已接收长度 
uint8_t RxFlag = 0; // 一帧接收完成标志（1=有新指令待处理）
void USART1_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出（TX）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入（RX） 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baud; // 波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1停止位 
	USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发都使能 
	USART_Init(USART1, &USART_InitStructure); 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能接收中断（RXNE=接收寄存器非空） 
	USART_Cmd(USART1, ENABLE); // 使能串口外设
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 子优先级 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler(void) 
	{ // 判断是否是「接收寄存器非空」中断
		if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) 
			{ uint8_t ch = USART_ReceiveData(USART1); // 读走收到的字节（同时清中断标志）
				if (ch == '\n'|ch=='\r') // 遇到换行符 → 一帧结束 
					{ RxBuf[RxLen] = '\0'; // 字符串结尾补0 
						RxFlag = 1; // 置标志位，通知主循环处理
						}
					else if (RxLen < 63) // 普通字符 → 存入缓冲区（防越界）
					{ RxBuf[RxLen++] = ch; 
				} 
		} 
	}
	
	// 重定向 fputc：printf 的每个字符都调用这个函数 
	int fputc(int ch, FILE *f)
		{ 
			USART_SendData(USART1, (uint8_t)ch); // 发送一个字节 
			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // 等发送完成
			return ch;
			
		}
		
		
#include "stm32f10x.h"
#include <stdio.h>

/* ===== 全局变量（接收缓冲区，供外部通过extern访问）===== */
uint8_t RxBuf[64];    /* 接收缓冲区，最多存63个字符（第64位存'\0'） */
uint8_t RxLen = 0;    /* 当前已接收长度 */
uint8_t RxFlag = 0;   /* 一帧接收完成标志（1=有新指令待主循环处理） */

/**
 * @brief  USART1 初始化（PA9=TX, PA10=RX，带接收中断+printf重定向）
 * @note   配置：8位数据、1停止位、无校验、无硬件流控、收发双工
 *         使能RXNE中断（接收寄存器非空时触发），在中断里逐字节接收
 *         遇到'\n'或'\r'时认为一帧结束，置RxFlag通知主循环
 * @param  baud 波特率，如 115200
 * @retval 无
 */
void USART1_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* PA9 = TX，复用推挽输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA10 = RX，浮空输入 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baud;                  /* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /* 8位数据 */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;       /* 1停止位 */
	USART_InitStructure.USART_Parity = USART_Parity_No;          /* 无校验 */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /* 收发都使能 */
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   /* 使能接收中断（RXNE=接收寄存器非空） */
	USART_Cmd(USART1, ENABLE);                          /* 使能串口外设 */

	/* 配置NVIC中断优先级 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  /* 抢占优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;          /* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART1 接收中断服务函数（每收到一个字节触发一次）
 * @note   中断流程：读走数据字节（同时清除RXNE标志）
 *         遇到换行符'\n'或回车'\r' → 一帧结束，补'\0'并置RxFlag
 *         普通字符 → 存入RxBuf，防越界（最多63个字符）
 * @param  无
 * @retval 无
 */
void USART1_IRQHandler(void)
{
	/* 判断是否是「接收寄存器非空」中断 */
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t ch = USART_ReceiveData(USART1);   /* 读走收到的字节（同时清中断标志） */

		if (ch == '\n' || ch == '\r')              /* 遇到换行/回车 → 一帧结束 */
		{
			RxBuf[RxLen] = '\0';                    /* 字符串结尾补0 */
			RxFlag = 1;                              /* 置标志位，通知主循环处理 */
		}
		else if (RxLen < 63)                        /* 普通字符 → 存入缓冲区（防越界） */
		{
			RxBuf[RxLen++] = ch;
		}
	}
}

/**
 * @brief  重定向 fputc：将 printf 的每个字符通过 USART1 发送
 * @note   这是标准库printf的底层输出函数，实现后即可直接用printf打印到串口
 *         发送后等待TXE标志（发送数据寄存器空），确保字符已发出
 * @param  ch 要发送的字符
 * @param  f 文件指针（printf重定向时固定，不使用）
 * @retval 发送的字符
 */
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t)ch);                  /* 发送一个字节 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  /* 等发送完成 */
	return ch;
}

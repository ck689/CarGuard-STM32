#include "stm32f10x.h"

/**
 * @brief  Serial 串口初始化（USART1，PA9=TX，仅发送模式，固定115200波特率）
 * @note   简化版串口驱动，只使能发送（TX），不使能接收和中断
 *         与USART.c功能重复，工程中实际使用USART.c的完整版
 * @param  无
 * @retval 无
 */
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* PA9 = TX，复用推挽输出 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;     /* 仅发送模式 */
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  Serial 发送一个字节（阻塞等待发送完成）
 * @param  Byte 要发送的字节数据
 * @retval 无
 */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  /* 等待发送寄存器空 */
}

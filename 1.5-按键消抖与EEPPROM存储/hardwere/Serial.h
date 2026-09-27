#ifndef _SERIAL_H
#define _SERIAL_H

/**
 * @brief  Serial 串口初始化（USART1仅发送，PA9=TX，固定115200）
 * @note   简化版驱动，工程中实际使用USART.c的完整版（带接收中断+printf）
 * @param  无
 * @retval 无
 */
void Serial_Init(void);

/**
 * @brief  Serial 发送一个字节（阻塞等待）
 * @param  Byte 要发送的字节
 * @retval 无
 */
void Serial_SendByte(uint8_t Byte);

#endif

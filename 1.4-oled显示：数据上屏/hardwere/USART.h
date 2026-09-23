#ifndef _USART_H
#define _USART_H

/* ===== 接收缓冲区全局变量（在USART.c中定义）===== */
extern uint8_t RxLen;     /* 当前已接收长度 */
extern uint8_t RxBuf[64]; /* 接收缓冲区，最多存63个字符 */
extern uint8_t RxFlag;    /* 一帧接收完成标志（1=有新指令待处理） */

/**
 * @brief  USART1 初始化（PA9=TX, PA10=RX，接收中断+printf重定向）
 * @param  baud 波特率，如 115200
 * @retval 无
 */
void USART1_Init(uint32_t baud);

/**
 * @brief  USART1 接收中断服务函数（逐字节接收，遇换行置RxFlag）
 * @param  无
 * @retval 无
 */
void USART1_IRQHandler(void);

#endif

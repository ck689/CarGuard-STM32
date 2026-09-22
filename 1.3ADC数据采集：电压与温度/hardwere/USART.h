extern uint8_t RxLen ; // 当前已接收长度 
extern uint8_t RxBuf[64]; // 接收缓冲区，最多存63个字符 
extern uint8_t RxFlag ; // 一帧接收完成标志（1=有新指令待处理）
#ifndef _USART_H
#define _USART_H
void USART1_Init(uint32_t baud);
void USART1_IRQHandler(void) ;
#endif

/**
 * @file   CAN.c
 * @brief  CAN 总线通信驱动（bxCAN控制器）
 * @note   V6 阶段实现：CAN初始化、标准/扩展帧收发、滤波器配置、回环模式
 *         V2 阶段预留文件，暂未实现具体函数
 *         硬件：PA11=CAN_RX, PA12=CAN_TX，需外接CAN收发器（TJA1050/SN65HVD230）
 */
#include "stm32f10x.h"

/* V6 待实现：
 * void CAN1_Init(void);                                    // CAN初始化（500kbps）
 * void CAN1_SendMsg(uint32_t id, uint8_t *data, uint8_t len);  // 发送CAN帧
 * uint8_t CAN1_GetMsg(uint32_t *id, uint8_t *data);      // 接收CAN帧（轮询）
 * void CAN1_RX0_IRQHandler(void);                          // CAN接收中断
 */

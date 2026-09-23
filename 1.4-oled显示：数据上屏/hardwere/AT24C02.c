/**
 * @file   AT24C02.c
 * @brief  AT24C02 EEPROM 驱动（I2C接口，2Kbit=256字节）
 * @note   V3 阶段实现：字节写、随机读、页写、参数掉电保存
 *         V2 阶段预留文件，暂未实现具体函数
 *         硬件：I2C地址 0xA0（写）/0xA1（读），与OLED共用软件I2C总线
 */
#include "stm32f10x.h"

/* V3 待实现：
 * void AT24C02_Init(void);
 * void AT24C02_WriteByte(uint8_t addr, uint8_t data);
 * uint8_t AT24C02_ReadByte(uint8_t addr);
 * void AT24C02_WritePage(uint8_t addr, uint8_t *buf, uint8_t len);
 */

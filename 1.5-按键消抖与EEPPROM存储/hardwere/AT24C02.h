#ifndef _AT24C02_H
#define _AT24C02_H

#include "stm32f10x.h"

/**
 * @file   AT24C02.h
 * @brief  AT24C02 EEPROM 驱动头文件
 * @note   V3 阶段实现。I2C地址0xA0(写)/0xA1(读)，256字节存储空间
 *         与OLED共用软件I2C总线(PB8=SCL, PB9=SDA)，靠器件地址区分
 */

/* ===== EEPROM 内存布局（256字节）===== */
#define EEPROM_FLAG_ADDR    0x00   /* 标志字节：0x5A=已初始化，其他=首次上电 */
#define EEPROM_FLAG_VALUE   0x5A
#define EEPROM_VOLT_TH_ADDR 0x01   /* 电压报警阈值（×10，如140表示14.0V） */
#define EEPROM_TEMP_TH_ADDR 0x02   /* 温度报警阈值（℃） */
/* 0x03~0x0F 预留 */
/* 0x10~0x7F 故障日志区（第10章用） */

#define DEFAULT_VOLT_TH     140    /* 默认电压阈值 14.0V */
#define DEFAULT_TEMP_TH     60     /* 默认温度阈值 60℃ */

/* ===== 全局参数变量（在 AT24C02.c 中定义）===== */
extern uint8_t volt_th;   /* 电压阈值（×10） */
extern uint8_t temp_th;   /* 温度阈值（℃） */

/* ===== 函数声明 ===== */

/**
 * @brief  向 AT24C02 指定地址写入一个字节
 * @param  addr: 内部地址 0~255
 * @param  data: 要写入的数据
 * @retval 无
 */
uint8_t AT24C02_WriteByte(uint8_t addr, uint8_t data);  /* 返回1=成功(收到ACK), 0=失败 */

/**
 * @brief  从 AT24C02 指定地址读取一个字节（随机读）
 * @param  addr: 内部地址 0~255
 * @retval 读到的数据
 */
uint8_t AT24C02_ReadByte(uint8_t addr);

/**
 * @brief  上电加载参数：首次上电写默认值，否则从EEPROM读取
 * @note   在 main 函数初始化阶段调用一次
 * @retval 无
 */
void Params_Load(void);

/**
 * @brief  保存当前阈值到 EEPROM（只在用户确认时调用，保护写寿命）
 * @retval 无
 */
void Params_Save(void);

#endif

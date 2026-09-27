#include "stm32f10x.h"
#include "Delay.h"
#include "SoftwareI2C.h"   /* 软件I2C底层：I2C_Start/Stop/SendByte/ReadByte/SendAck */
#include "AT24C02.h"
#include <stdio.h>         /* printf */

/* ===== 全局参数变量 ===== */
uint8_t volt_th = DEFAULT_VOLT_TH;   /* 电压阈值（×10，默认14.0V） */
uint8_t temp_th = DEFAULT_TEMP_TH;   /* 温度阈值（℃，默认60℃） */

/**
 * @brief  向 AT24C02 指定地址写入一个字节
 * @note   时序：起始 → 写地址0xA0 → 内部地址 → 数据 → 停止 → 等5ms
 *         写周期5ms必须等，否则下一次写会失败
 */
uint8_t AT24C02_WriteByte(uint8_t addr, uint8_t data)
{
	uint8_t ack;
	I2C_Start();
	ack = I2C_SendByte(0xA0);    /* AT24C02 写地址 */
	if (!ack) { I2C_Stop(); printf("[EEPROM] 写失败：器件无应答(地址0xA0)\r\n"); return 0; }
	ack = I2C_SendByte(addr);    /* 内部字节地址（0~255） */
	if (!ack) { I2C_Stop(); printf("[EEPROM] 写失败：内部地址无应答\r\n"); return 0; }
	ack = I2C_SendByte(data);    /* 要写入的数据 */
	if (!ack) { I2C_Stop(); printf("[EEPROM] 写失败：数据无应答\r\n"); return 0; }
	I2C_Stop();
	Delay_ms(5);                 /* 等待内部写周期（必须！） */
	return 1;                    /* 1=成功 */
}

/**
 * @brief  从 AT24C02 指定地址读取一个字节（随机读）
 * @note   时序：起始 → 写地址+内部地址 → 重复起始 → 读地址0xA1 → 读数据 → NACK → 停止
 */
uint8_t AT24C02_ReadByte(uint8_t addr)
{
	uint8_t data;

	I2C_Start();
	I2C_SendByte(0xA0);          /* 写操作：设置内部地址指针 */
	I2C_SendByte(addr);
	I2C_Start();                 /* 重复起始（Repeated Start） */
	I2C_SendByte(0xA1);          /* 读操作 */
	data = I2C_ReadByte();       /* 读一个字节 */
	I2C_SendAck(0);              /* 发送 NACK（非应答，表示不再继续读） */
	I2C_Stop();

	return data;
}

/**
 * @brief  上电加载参数
 * @note   读标志字节：不是0x5A说明是首次上电（EEPROM出厂为0xFF），写入默认值；
 *         是0x5A说明已初始化，从EEPROM读取用户保存的阈值
 */
void Params_Load(void)
{
	uint8_t flag = AT24C02_ReadByte(EEPROM_FLAG_ADDR);
	printf("[Params] EEPROM标志字节=0x%02X\r\n", flag);
	if (flag != EEPROM_FLAG_VALUE)
	{
		/* 第一次上电（EEPROM 为空）→ 写入默认值 */
		uint8_t ok = 1;
		ok &= AT24C02_WriteByte(EEPROM_FLAG_ADDR, EEPROM_FLAG_VALUE);
		ok &= AT24C02_WriteByte(EEPROM_VOLT_TH_ADDR, DEFAULT_VOLT_TH);
		ok &= AT24C02_WriteByte(EEPROM_TEMP_TH_ADDR, DEFAULT_TEMP_TH);
		if (ok)
			printf("[Params] 首次上电，已写入默认阈值\r\n");
		else
			printf("[Params] 首次上电写默认值失败! 检查AT24C02硬件\r\n");
	}
	else
	{
		/* 已初始化 → 从 EEPROM 读取 */
		volt_th = AT24C02_ReadByte(EEPROM_VOLT_TH_ADDR);
		temp_th = AT24C02_ReadByte(EEPROM_TEMP_TH_ADDR);
		printf("[Params] 加载阈值: Volt=%.1fV Temp=%dC\r\n", volt_th / 10.0f, temp_th);
	}
}

/**
 * @brief  保存当前阈值到 EEPROM
 * @note   只在用户确认按键时调用，不要在主循环里频繁写（写寿命约10万次）
 */
void Params_Save(void)
{
	uint8_t ok = 1;
	ok &= AT24C02_WriteByte(EEPROM_VOLT_TH_ADDR, volt_th);
	ok &= AT24C02_WriteByte(EEPROM_TEMP_TH_ADDR, temp_th);
	if (ok)
	{
		/* 写后立即读回验证 */
		uint8_t v = AT24C02_ReadByte(EEPROM_VOLT_TH_ADDR);
		uint8_t t = AT24C02_ReadByte(EEPROM_TEMP_TH_ADDR);
		if (v == volt_th && t == temp_th)
			printf("[Params] 保存成功并验证: Volt=%.1fV Temp=%dC\r\n", volt_th / 10.0f, temp_th);
		else
			printf("[Params] 写后读回不一致! 写入=%d/%d 读出=%d/%d (检查硬件)\r\n", volt_th, temp_th, v, t);
	}
	else
	{
		printf("[Params] 保存失败! 检查AT24C02接线:A0/A1/A2接地, WP接地, SDA/SCL上拉\r\n");
	}
}

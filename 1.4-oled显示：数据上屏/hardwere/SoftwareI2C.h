#ifndef _SOFTWAREI2C_H
#define _SOFTWAREI2C_H

/**
 * @brief  软件模拟 I2C 初始化（PB8=SCL, PB9=SDA，开漏输出）
 * @param  无
 * @retval 无
 */
void I2C1_Init(void);

/**
 * @brief  产生 I2C 起始条件（SCL高电平时，SDA从高变低）
 * @param  无
 * @retval 无
 */
void I2C_Start(void);

/**
 * @brief  产生 I2C 停止条件（SCL高电平时，SDA从低变高）
 * @param  无
 * @retval 无
 */
void I2C_Stop(void);

/**
 * @brief  I2C 发送一个字节（高位先发），并读取从机应答
 * @param  data 要发送的字节数据
 * @retval 1=收到ACK(从机应答), 0=收到NACK(从机无应答)
 */
uint8_t I2C_SendByte(uint8_t data);

#endif

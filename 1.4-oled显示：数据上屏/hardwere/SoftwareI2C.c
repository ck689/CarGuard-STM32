#include "stm32f10x.h"
#include "Delay.h"

/* ===== 引脚位操作宏（直接操作寄存器，比库函数快）===== */
#define SCL_HIGH()  GPIOB->BSRR = GPIO_Pin_8   /* SCL 拉高 */
#define SCL_LOW()   GPIOB->BRR  = GPIO_Pin_8   /* SCL 拉低 */
#define SDA_HIGH()  GPIOB->BSRR = GPIO_Pin_9   /* SDA 拉高 */
#define SDA_LOW()   GPIOB->BRR  = GPIO_Pin_9   /* SDA 拉低 */
#define SDA_READ()  (GPIOB->IDR & GPIO_Pin_9)  /* 读取 SDA 电平 */

/**
 * @brief  软件模拟 I2C 初始化
 * @note   PB8=SCL, PB9=SDA，配置为开漏输出（I2C必须开漏，靠外部上拉电阻产生高电平）
 *         开漏输出可防止多设备挂总线时"一个拉低一个拉高"造成短路
 * @param  无
 * @retval 无
 */
void I2C1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   /* 开漏输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SCL_HIGH();
	SDA_HIGH();   /* 总线空闲状态：SCL和SDA都拉高 */
}

/**
 * @brief  产生 I2C 起始条件 START
 * @note   时序：SCL高电平时，SDA从高电平跳变为低电平
 *         起始后拉低SCL，准备发送数据
 * @param  无
 * @retval 无
 */
void I2C_Start(void)
{
	SDA_HIGH();
	SCL_HIGH();       /* 先确保总线空闲（都高） */
	Delay_us(5);
	SDA_LOW();         /* SDA先变低 → 起始条件 */
	Delay_us(5);
	SCL_LOW();         /* 拉低SCL，准备发数据 */
}

/**
 * @brief  产生 I2C 停止条件 STOP
 * @note   时序：SCL高电平时，SDA从低电平跳变为高电平
 * @param  无
 * @retval 无
 */
void I2C_Stop(void)
{
	SDA_LOW();
	SCL_HIGH();
	Delay_us(5);
	SDA_HIGH();        /* SDA变高 → 停止条件 */
	Delay_us(5);
}

/**
 * @brief  I2C 发送一个字节（高位先发，MSB First）
 * @note   发送8位数据后，在第9个时钟读取从机的ACK应答
 *         从机拉低SDA表示ACK(收到)，SDA保持高表示NACK(未收到)
 * @param  data 要发送的字节数据
 * @retval 1=收到ACK, 0=收到NACK
 */
uint8_t I2C_SendByte(uint8_t data)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		/* SCL低电平时改变SDA数据 */
		if (data & 0x80)
			SDA_HIGH();    /* 先发最高位 */
		else
			SDA_LOW();
		data <<= 1;
		Delay_us(5);
		SCL_HIGH();        /* SCL高电平，从机读取数据位 */
		Delay_us(5);
		SCL_LOW();         /* SCL低电平，准备下一位 */
	}

	/* 第9个时钟：读取从机应答 ACK */
	SDA_HIGH();            /* 释放SDA（开漏，由从机控制电平） */
	Delay_us(5);
	SCL_HIGH();
	Delay_us(5);
	uint8_t ack = SDA_READ() ? 0 : 1;   /* SDA低=ACK(1), SDA高=NACK(0) */
	SCL_LOW();

	return ack;
}

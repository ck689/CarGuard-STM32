#include "stm32f10x.h"
#include "Delay.h"

/**
 * @brief  按键初始化（PB1=KEY1, PB11=KEY2，内部上拉输入）
 * @note   按键一端接引脚，另一端接GND；未按下时引脚为高电平（内部上拉），按下时为低电平
 * @param  无
 * @retval 无
 */
void key_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     /* 上拉输入（内部上拉） */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief  扫描按键并返回键值（阻塞式消抖，等待按键释放）
 * @note   消抖策略：检测到低电平后延时20ms消抖，然后等待松手，再延时20ms
 *         每次调用只返回一个键值，同时按下时KEY2优先
 * @param  无
 * @retval 0=无按键, 1=KEY1(PB1)按下, 2=KEY2(PB11)按下
 */
uint8_t key_getnum(void)
{
	uint8_t keynum = 0;

	/* 检测 KEY1（PB1） */
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		Delay_ms(20);                              /* 消抖延时 */
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);  /* 等待松手 */
		Delay_ms(20);                              /* 松手消抖 */
		keynum = 1;
	}

	/* 检测 KEY2（PB11） */
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
		Delay_ms(20);
		keynum = 2;
	}

	return keynum;
}

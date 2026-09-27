#include "stm32f10x.h"
#include "Delay.h"
#include "key.h"

/**
 * @brief  按键 GPIO 初始化（内部上拉输入）
 * @note   PB4=KEY1, PB5=KEY2，按键一端接IO，另一端接GND
 *         内部上拉：未按下时引脚为高电平，按下时为低电平
 */
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	/* PB4默认是JTAG的NJTRST引脚，必须禁用JTAG才能当普通GPIO用（保留SWD下载） */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;   /* 上拉输入（内部上拉） */
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief  按键扫描（状态消抖法，检测下降沿）
 * @note   用静态变量记录上次状态，检测"高→低"的下降沿，
 *         检测到低电平后延时10ms再确认，确认后才算一次有效按下。
 *         松开后（恢复高电平）重置状态，等待下一次按下。
 *         V3阶段在主循环里调用；V4会移到1ms定时器中断里。
 * @retval 0=无按键, 1=KEY1按下, 2=KEY2按下
 */
uint8_t Key_Scan(void)
{
	static uint8_t key1_state = 1;   /* 按键1上次状态（1=未按下） */
	static uint8_t key2_state = 1;   /* 按键2上次状态（1=未按下） */
	uint8_t result = KEY_NONE;

	/* ===== 按键1（PB4）：检测下降沿 + 消抖 ===== */
	if (key1_state == 1 && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
	{
		Delay_ms(10);   /* 消抖延时（V4阶段改为定时器计数，不用Delay） */
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
		{
			result = KEY1_PRESS;
			key1_state = 0;   /* 标记为已按下，防止按住重复触发 */
		}
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 1)
		key1_state = 1;       /* 松开后恢复，等待下一次按下 */

	/* ===== 按键2（PB5）：同理 ===== */
	if (key2_state == 1 && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0)
	{
		Delay_ms(10);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0)
		{
			result = KEY2_PRESS;
			key2_state = 0;
		}
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 1)
		key2_state = 1;

	return result;
}

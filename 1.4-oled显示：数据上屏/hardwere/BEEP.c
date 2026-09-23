#include "stm32f10x.h"

static uint8_t beep_state = 0;   /* 保存蜂鸣器当前状态（供BEEP_Get读取） */

/**
 * @brief  蜂鸣器初始化（PB0 推挽输出，低电平鸣叫）
 * @note   初始状态为熄灭（PB0置高）
 * @param  无
 * @retval 无
 */
void BEEP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   /* 推挽输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_0);   /* 初始熄灭（高电平） */
}

/**
 * @brief  设置蜂鸣器状态
 * @param  state 1=鸣叫（PB0拉低）, 0=停止（PB0拉高）
 * @retval 无
 */
void BEEP_Set(uint8_t state)
{
	beep_state = state;
	if (state)
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);   /* 低电平鸣叫 */
	else
		GPIO_SetBits(GPIOB, GPIO_Pin_0);      /* 高电平停止 */
}

/**
 * @brief  获取蜂鸣器当前状态
 * @param  无
 * @retval 1=正在鸣叫, 0=停止
 */
uint8_t BEEP_Get(void)
{
	return beep_state;
}

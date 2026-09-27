#include "stm32f10x.h"

static uint8_t led_state = 0;   /* 保存LED当前状态（供LED_Get读取） */

/**
 * @brief  LED 初始化（PA8 推挽输出，高电平点亮）
 * @note   初始状态为熄灭（PA8置低）
 * @param  无
 * @retval 无
 */
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   /* 推挽输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_8);   /* 初始熄灭（低电平） */
}

/**
 * @brief  设置LED状态
 * @param  state 1=点亮（PA8拉高）, 0=熄灭（PA8拉低）
 * @retval 无
 */
void LED_Set(uint8_t state)
{
	led_state = state;
	if (state)
		GPIO_SetBits(GPIOA, GPIO_Pin_8);     /* 高电平点亮 */
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);    /* 低电平熄灭 */
}

/**
 * @brief  获取LED当前状态
 * @param  无
 * @retval 1=点亮, 0=熄灭
 */
uint8_t LED_Get(void)
{
	return led_state;
}

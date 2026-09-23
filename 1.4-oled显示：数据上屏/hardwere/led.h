#ifndef _LED_H
#define _LED_H

/**
 * @brief  LED 初始化（PA8 推挽输出，高电平点亮）
 * @param  无
 * @retval 无
 */
void LED_Init(void);

/**
 * @brief  设置LED状态
 * @param  state 1=点亮, 0=熄灭
 * @retval 无
 */
void LED_Set(uint8_t state);

/**
 * @brief  获取LED当前状态
 * @param  无
 * @retval 1=点亮, 0=熄灭
 */
uint8_t LED_Get(void);

#endif

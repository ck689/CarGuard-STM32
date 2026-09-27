#ifndef _BEEP_H
#define _BEEP_H

/**
 * @brief  蜂鸣器初始化（PB0 推挽输出，低电平有效）
 * @param  无
 * @retval 无
 */
void BEEP_Init(void);

/**
 * @brief  设置蜂鸣器状态
 * @param  state 1=鸣叫, 0=停止
 * @retval 无
 */
void BEEP_Set(uint8_t state);

/**
 * @brief  获取蜂鸣器当前状态
 * @param  无
 * @retval 1=鸣叫中, 0=停止
 */
uint8_t BEEP_Get(void);

#endif

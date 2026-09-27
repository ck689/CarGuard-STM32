#ifndef _KEY_H
#define _KEY_H

#include "stm32f10x.h"

/* ===== 按键返回值定义 ===== */
#define KEY_NONE    0   /* 无按键 */
#define KEY1_PRESS  1   /* 按键1按下 */
#define KEY2_PRESS  2   /* 按键2按下 */

/**
 * @brief  按键初始化（PB4=KEY1, PB5=KEY2，内部上拉输入）
 * @note   按键一端接IO，另一端接GND；未按下为高电平，按下为低电平
 * @retval 无
 */
void Key_Init(void);

/**
 * @brief  扫描按键（状态消抖法，检测下降沿，按一次返回一次）
 * @retval 0=无按键, 1=KEY1, 2=KEY2
 */
uint8_t Key_Scan(void);

#endif

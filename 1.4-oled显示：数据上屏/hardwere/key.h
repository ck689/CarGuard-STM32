#ifndef _key_H
#define _key_H

/**
 * @brief  按键初始化（PB1=KEY1, PB11=KEY2，内部上拉输入）
 * @param  无
 * @retval 无
 */
void key_init(void);

/**
 * @brief  扫描按键（阻塞式消抖，等待释放）
 * @param  无
 * @retval 0=无按键, 1=KEY1, 2=KEY2
 */
uint8_t key_getnum(void);

#endif

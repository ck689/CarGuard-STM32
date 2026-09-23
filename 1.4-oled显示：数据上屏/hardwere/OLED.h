#ifndef __OLED_H
#define __OLED_H

/**
 * @brief  OLED 初始化（SSD1306，软件I2C，PB8=SCL, PB9=SDA）
 * @param  无
 * @retval 无
 */
void OLED_Init(void);

/**
 * @brief  OLED 清屏（全部像素熄灭）
 * @param  无
 * @retval 无
 */
void OLED_Clear(void);

/**
 * @brief  OLED 显示一个ASCII字符（8x16点阵）
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Char 要显示的ASCII字符
 * @retval 无
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);

/**
 * @brief  OLED 显示字符串
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  String 以'\0'结尾的字符串
 * @retval 无
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);

/**
 * @brief  OLED 显示无符号十进制整数（高位补0）
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字
 * @param  Length 显示位数
 * @retval 无
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

/**
 * @brief  OLED 显示有符号十进制整数（自动+/-号）
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字
 * @param  Length 数字部分位数（不含符号）
 * @retval 无
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);

/**
 * @brief  OLED 显示十六进制数（大写）
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字
 * @param  Length 显示位数
 * @retval 无
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

/**
 * @brief  OLED 显示二进制数
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字
 * @param  Length 显示位数
 * @retval 无
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

/**
 * @brief  OLED 显示浮点数（符号+整数+小数点+小数，四舍五入）
 * @note   总占位 = 符号位(1) + IntLen + 小数点(1) + DecLen
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的浮点数
 * @param  IntLen 整数部分位数（不含符号）
 * @param  DecLen 小数部分位数
 * @retval 无
 */
void OLED_ShowFloatNum(uint8_t Line, uint8_t Column, float Number, uint8_t IntLen, uint8_t DecLen);

#endif

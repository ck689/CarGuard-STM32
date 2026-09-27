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
 * @brief  OLED 显示一个汉字（16x16点阵）
 * @note   汉字占2页(16像素高)×16列(16像素宽)，即宽度为2个ASCII字符
 *         汉字必须已收录在 OLED_Font.h 的 OLED_ChineseIndex / OLED_F16x16 中
 *         【重要】源文件必须保存为 UTF-8 编码，否则汉字会乱码！
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16（汉字占2列，如Column=1则占1~2列）
 * @param  Chinese 指向汉字的指针（UTF-8编码占3字节，传字符串首地址即可）
 * @retval 无
 */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, char *Chinese);

/**
 * @brief  OLED 显示字符串（支持中文与ASCII混合）
 * @note   自动识别（UTF-8）：字节≥0x80 视为UTF-8中文（占3字节、占2列宽度），否则视为ASCII
 *         未收录的汉字将显示为空白
 *         【重要】源文件必须保存为 UTF-8 编码，否则汉字会乱码！
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  String 以'\0'结尾的字符串（可含中文）
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

#include "stm32f10x.h"
#include "Delay.h"
#include "SoftwareI2C.h"   /* 软件I2C底层：I2C1_Init/I2C_Start/I2C_Stop/I2C_SendByte */
#include "OLED_Font.h"      /* ASCII 8x16 字库 */

/**
 * @brief                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    OLED 写命令（控制字节=0x00，表示后续字节是命令）
 * @param  cmd 要写入的命令字节
 * @retval 无
 */
void OLED_WriteCmd(uint8_t cmd)
{
	I2C_Start();
	I2C_SendByte(0x78);    /* OLED I2C 写地址（SSD1306固定为0x78） */
	I2C_SendByte(0x00);    /* 控制字节：0x00 = 后面是命令 */
	I2C_SendByte(cmd);     /* 命令内容 */
	I2C_Stop();
}

/**
 * @brief  OLED 写数据（控制字节=0x40，表示后续字节是显存数据）
 * @param  data 要写入显存的数据字节（每字节对应一列8个像素点）
 * @retval 无
 */
void OLED_WriteData(uint8_t data)
{
	I2C_Start();
	I2C_SendByte(0x78);    /* OLED I2C 写地址 */
	I2C_SendByte(0x40);    /* 控制字节：0x40 = 后面是数据（显存内容） */
	I2C_SendByte(data);    /* 数据内容 */
	I2C_Stop();
}

/**
 * @brief  设置 OLED 光标位置（页地址模式）
 * @note   SSD1306 将128x64屏幕分为8页(Page0~7)，每页8行高、128列宽
 *         本函数使用原始坐标：page=0~7, column=0~127
 * @param  page 页地址，范围：0~7
 * @param  column 列地址，范围：0~127
 * @retval 无
 */
void OLED_SetCursor(uint8_t page, uint8_t column)
{
	OLED_WriteCmd(0xB0 | page);              /* 设置页地址（0xB0 + page） */
	OLED_WriteCmd(0x10 | ((column & 0xF0) >> 4));  /* 列地址高4位 */
	OLED_WriteCmd(0x00 | (column & 0x0F));           /* 列地址低4位 */
}

/**
 * @brief  OLED 清屏（将全部8页×128列的显存写0）
 * @param  无
 * @retval 无
 */
void OLED_Clear(void)
{
	uint8_t page, col;
	for (page = 0; page < 8; page++)
	{
		OLED_SetCursor(page, 0);
		for (col = 0; col < 128; col++)
		{
			OLED_WriteData(0x00);   /* 写0 = 该列8个像素全部熄灭 */
		}
	}
}

/**
 * @brief  OLED 显示一个字符（8x16 点阵，ASCII可见字符）
 * @note   字符占2页(16像素高)×8列(8像素宽)
 *         江协坐标：Line=1~4, Column=1~16
 *         转换为原始坐标：page=(Line-1)*2, column=(Column-1)*8
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Char 要显示的ASCII字符
 * @retval 无
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	/* 上半页（字符的上8行） */
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);
	}
	/* 下半页（字符的下8行） */
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
	}
}

/**
 * @brief  OLED 显示一个汉字（16x16 点阵）
 * @note   在 OLED_ChineseIndex 索引表中按 UTF-8 编码逐字查找，
 *         找到后取对应下标的 OLED_F16x16 字模上屏。
 *         汉字宽度=2个ASCII字符，占连续2列。
 *         【重要】源文件必须保存为 UTF-8 编码，否则汉字会乱码！
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Chinese 指向汉字的指针（UTF-8编码3字节，传&字符串[i]即可）
 * @retval 无
 */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, char *Chinese)
{
	uint8_t i, j;
	uint8_t index = 0;

	/* 在汉字索引表中查找：每个汉字占3字节(UTF-8)，逐字比较3个字节 */
	for (i = 0; OLED_ChineseIndex[i] != '\0'; i += 3)
	{
		if (Chinese[0] == OLED_ChineseIndex[i] &&
		    Chinese[1] == OLED_ChineseIndex[i + 1] &&
		    Chinese[2] == OLED_ChineseIndex[i + 2])
		{
			index = i / 3;   /* 命中，计算字模数组下标 */

			/* 上半页（汉字第0~7行，共16列） */
			OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
			for (j = 0; j < 16; j++)
			{
				OLED_WriteData(OLED_F16x16[index][j]);
			}
			/* 下半页（汉字第8~15行，共16列） */
			OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
			for (j = 0; j < 16; j++)
			{
				OLED_WriteData(OLED_F16x16[index][j + 16]);
			}
			return;
		}
	}
	/* 未找到该汉字（字库未收录），则什么都不显示（留空） */
}

/**
 * @brief  OLED 显示字符串（支持中文与ASCII混合显示）
 * @note   自动识别编码（UTF-8）：
 *           - 字节 < 0x80 → ASCII字符，占1字节1列，调用 OLED_ShowChar
 *           - 字节 ≥ 0x80 → UTF-8中文首字节(1110xxxx)，与后2字节组成汉字，占3字节2列，调用 OLED_ShowChinese
 *         【重要】源文件必须保存为 UTF-8 编码，直接写中文字符串即可，如 OLED_ShowString(1,1,"车速:120");
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  String 以'\0'结尾的字符串（可含中文）
 * @retval 无
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i = 0;
	uint8_t col = Column;
	while (String[i] != '\0')
	{
		if ((uint8_t)String[i] >= 0x80)   /* UTF-8中文：首字节≥0x80，占3字节 */
		{
			OLED_ShowChinese(Line, col, &String[i]);
			i += 3;
			col += 2;                       /* 汉字宽度=2个ASCII字符 */
		}
		else                                /* ASCII字符：占1字节1列 */
		{
			OLED_ShowChar(Line, col, String[i]);
			i++;
			col++;
		}
	}
}

/**
 * @brief  OLED 内部次方函数（计算 X 的 Y 次方，供数字显示函数使用）
 * @param  X 底数
 * @param  Y 指数
 * @retval X^Y 的结果
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
 * @brief  OLED 显示无符号十进制整数
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字，范围：0~4294967295
 * @param  Length 显示位数（高位不足补0），范围：1~10
 * @retval 无
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED 显示有符号十进制整数（自动显示+/-号）
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字，范围：-2147483648~2147483647
 * @param  Length 数字部分位数（不含符号位），范围：1~10
 * @retval 无
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED 显示十六进制数（大写A~F）
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
 * @param  Length 显示位数，范围：1~8
 * @retval 无
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
 * @brief  OLED 显示二进制数
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的数字
 * @param  Length 显示位数，范围：1~16
 * @retval 无
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
 * @brief  OLED 显示浮点数（符号+整数+小数点+小数，四舍五入）
 * @note   总占位 = 符号位(1) + IntLen + 小数点(1) + DecLen
 *         例如 IntLen=2, DecLen=2 → 总宽6列：" 12.34" 或 "-12.34"
 *         正数用空格占位以保持与负数对齐
 * @param  Line 起始行，范围：1~4
 * @param  Column 起始列，范围：1~16
 * @param  Number 要显示的浮点数
 * @param  IntLen 整数部分位数（不含符号），范围：1~9
 * @param  DecLen 小数部分位数，范围：1~4
 * @retval 无
 */
void OLED_ShowFloatNum(uint8_t Line, uint8_t Column, float Number, uint8_t IntLen, uint8_t DecLen)
{
	uint8_t col = Column;
	uint32_t intPart, decPart;
	float scale = 1.0f;
	uint8_t i;

	/* ① 符号位：负数显示'-'，正数显示空格保持对齐 */
	if (Number < 0)
	{
		OLED_ShowChar(Line, col, '-');
		Number = -Number;
	}
	else
	{
		OLED_ShowChar(Line, col, ' ');
	}
	col++;

	/* ② 计算小数放大倍数（10^DecLen） */
	for (i = 0; i < DecLen; i++)
	{
		scale *= 10.0f;
	}

	/* ③ 四舍五入到指定小数位 */
	uint32_t scaled = (uint32_t)(Number * scale + 0.5f);

	/* ④ 分离整数部分和小数部分 */
	intPart = scaled / (uint32_t)scale;
	decPart = scaled % (uint32_t)scale;

	/* ⑤ 显示整数部分（高位补0） */
	for (i = 0; i < IntLen; i++)
	{
		OLED_ShowChar(Line, col + i, '0' + intPart / OLED_Pow(10, IntLen - i - 1) % 10);
	}
	col += IntLen;

	/* ⑥ 显示小数点 */
	OLED_ShowChar(Line, col, '.');
	col++;

	/* ⑦ 显示小数部分（高位补0） */
	for (i = 0; i < DecLen; i++)
	{
		OLED_ShowChar(Line, col + i, '0' + decPart / OLED_Pow(10, DecLen - i - 1) % 10);
	}
}

/**
 * @brief  OLED 初始化（SSD1306 控制器标准初始化序列）
 * @note   流程：I2C初始化 → 延时等待上电 → 发送初始化命令序列 → 清屏 → 开启显示
 *         初始化命令照着SSD1306数据手册写即可，无需死记
 * @param  无
 * @retval 无
 */
void OLED_Init(void)
{
	I2C1_Init();              /* 初始化软件I2C（PB8=SCL, PB9=SDA） */
	Delay_ms(100);            /* 等待OLED上电稳定 */

	OLED_WriteCmd(0xAE);      /* 关闭显示（初始化期间先关屏） */

	OLED_WriteCmd(0xD5);      /* 设置显示时钟分频比/振荡器频率 */
	OLED_WriteCmd(0x80);

	OLED_WriteCmd(0xA8);      /* 设置多路复用率（64行 = 0x3F） */
	OLED_WriteCmd(0x3F);

	OLED_WriteCmd(0xD3);      /* 设置显示偏移 */
	OLED_WriteCmd(0x00);

	OLED_WriteCmd(0x40);      /* 设置显示开始行（从第0行开始） */

	OLED_WriteCmd(0x8D);      /* 电荷泵设置 */
	OLED_WriteCmd(0x14);      /* 使能电荷泵（OLED自升压，必须开启） */

	OLED_WriteCmd(0x20);      /* 设置内存地址模式 */
	OLED_WriteCmd(0x02);      /* 页地址模式（Page Addressing Mode） */

	OLED_WriteCmd(0xA1);      /* 段重映射（左右方向，0xA1=正常） */

	OLED_WriteCmd(0xC8);      /* 扫描方向（上下方向，0xC8=正常） */

	OLED_WriteCmd(0xDA);      /* 设置COM引脚硬件配置 */
	OLED_WriteCmd(0x12);

	OLED_WriteCmd(0x81);      /* 设置对比度控制 */
	OLED_WriteCmd(0xCF);

	OLED_WriteCmd(0xD9);      /* 设置预充电周期 */
	OLED_WriteCmd(0xF1);

	OLED_WriteCmd(0xDB);      /* 设置VCOMH取消选择级别 */
	OLED_WriteCmd(0x30);

	OLED_WriteCmd(0xA4);      /* 全局显示开（跟随显存内容） */

	OLED_WriteCmd(0xA6);      /* 正常显示（非反色，0xA7=反色） */

	OLED_WriteCmd(0xAF);      /* 开启显示 */

	OLED_Clear();              /* 清屏，避免上电残留乱码 */
}

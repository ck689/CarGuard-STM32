# -*- coding: utf-8 -*-
"""
OLED 16x16 中文字模生成脚本
- 使用 Windows 系统宋体 (simsun.ttc) 渲染汉字
- 输出格式：列行式、阴码(1=点亮)、低位在上(D0=第0行)
- 每个汉字 32 字节：前16字节=上半页(行0~7)，后16字节=下半页(行8~15)
- 与项目中 OLED_F8x16 ASCII 字库格式完全一致
"""

from PIL import Image, ImageDraw, ImageFont
import os

# ============== 配置区 ==============
# 需要生成字模的汉字（车载终端常用字，去重）
CHARS = "智能车载终端系统电压温度时间日期里程油耗转速警告状态开始停止设置故障正常数据显示导航蓝牙音乐电话信号"

# 系统宋体路径
FONT_PATH = r"C:\Windows\Fonts\simsun.ttc"

# 输出字模尺寸
FONT_SIZE = 16
CANVAS_SIZE = 16
# ====================================

def render_char(ch, font_path, font_size=16, canvas_size=16):
    """渲染单个汉字为 16x16 二值图像，居中"""
    # 创建黑色背景画布
    img = Image.new('1', (canvas_size, canvas_size), 0)
    draw = ImageDraw.Draw(img)
    try:
        font = ImageFont.truetype(font_path, font_size)
    except Exception:
        font = ImageFont.load_default()

    # 获取文字边界框
    bbox = draw.textbbox((0, 0), ch, font=font)
    w = bbox[2] - bbox[0]
    h = bbox[3] - bbox[1]
    # 居中绘制
    x = (canvas_size - w) // 2 - bbox[0]
    y = (canvas_size - h) // 2 - bbox[1]
    draw.text((x, y), ch, font=font, fill=1)
    return img

def img_to_fontdata(img):
    """
    将 16x16 二值图转换为列行式字模数据（32字节）
    每字节对应一列8个像素，D0=最上行，D7=最下行
    前16字节 = 上半页(第0~7行)，后16字节 = 下半页(第8~15行)
    """
    pixels = img.load()
    data = []
    # 上半页：行0~7
    for col in range(16):
        byte = 0
        for row in range(8):
            if pixels[col, row]:
                byte |= (1 << row)   # D0=第0行(最上)
        data.append(byte)
    # 下半页：行8~15
    for col in range(16):
        byte = 0
        for row in range(8, 16):
            if pixels[col, row]:
                byte |= (1 << (row - 8))
        data.append(byte)
    return data

def main():
    # 去重并保持顺序
    seen = set()
    unique_chars = []
    for ch in CHARS:
        if ch not in seen:
            seen.add(ch)
            unique_chars.append(ch)

    print(f"共 {len(unique_chars)} 个汉字")
    print(f"字模索引字符串: \"{''.join(unique_chars)}\"")
    print()

    # 生成每个字的字模
    all_data = []
    for ch in unique_chars:
        img = render_char(ch, FONT_PATH, FONT_SIZE, CANVAS_SIZE)
        data = img_to_fontdata(img)
        all_data.append((ch, data))

    # 输出 C 代码
    print("/* 汉字索引表（与字模数组顺序一一对应） */")
    print(f'const char OLED_ChineseIndex[] = "{ "".join(unique_chars) }";')
    print()
    print("/* 中文字模数组，每个汉字32字节 */")
    print("const uint8_t OLED_F16x16[][32] =")
    print("{")
    for ch, data in all_data:
        hex_str = ",".join(f"0x{b:02X}" for b in data)
        print(f"    {{{hex_str}}},/* {ch} */")
    print("};")

if __name__ == "__main__":
    main()

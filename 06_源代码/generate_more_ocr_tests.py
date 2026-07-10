from pathlib import Path

from PIL import Image, ImageDraw, ImageEnhance, ImageFilter, ImageFont


OUT = Path("test_samples_more")
OUT.mkdir(parents=True, exist_ok=True)


def font(size=40):
    candidates = [
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/msyhbd.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/arial.ttf",
    ]
    for path in candidates:
        try:
            return ImageFont.truetype(path, size)
        except OSError:
            continue
    return ImageFont.load_default()


def save_card(name, title, lines, size=(980, 520), bg=(252, 253, 255), fg=(30, 42, 58),
              title_color=(82, 96, 116), rotate=0, blur=0, contrast=1.0):
    img = Image.new("RGB", size, bg)
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle((24, 24, size[0] - 24, size[1] - 24), radius=18, outline=(200, 210, 224), width=2)
    draw.text((52, 42), title, fill=title_color, font=font(26))
    y = 116
    for line in lines:
        draw.text((72, y), line, fill=fg, font=font(40))
        y += 68
    if contrast != 1.0:
        img = ImageEnhance.Contrast(img).enhance(contrast)
    if blur:
        img = img.filter(ImageFilter.GaussianBlur(blur))
    if rotate:
        img = img.rotate(rotate, expand=True, fillcolor=bg)
    img.save(OUT / name)


def save_table(name):
    img = Image.new("RGB", (1050, 560), (255, 255, 255))
    draw = ImageDraw.Draw(img)
    draw.text((42, 32), "20 Table with Chinese fields", fill=(82, 96, 116), font=font(26))
    x0, y0 = 52, 96
    cols = [0, 270, 560, 820, 960]
    rows = [0, 72, 144, 216, 288, 360]
    for c in cols:
        draw.line((x0 + c, y0, x0 + c, y0 + rows[-1]), fill=(70, 82, 98), width=2)
    for r in rows:
        draw.line((x0, y0 + r, x0 + cols[-1], y0 + r), fill=(70, 82, 98), width=2)
    table_rows = [
        ["姓名", "课程", "分数", "备注"],
        ["张三", "机器人视觉", "92", "通过"],
        ["李四", "图像处理", "88", "通过"],
        ["王五", "OCR识别", "95", "优秀"],
    ]
    for i, row in enumerate(table_rows):
        for j, text in enumerate(row):
            draw.text((x0 + cols[j] + 24, y0 + rows[i] + 18), text, fill=(28, 38, 52), font=font(30))
    img.save(OUT / name)


save_card(
    "17_clean_chinese_notice.png",
    "17 Clean Chinese Notice",
    ["实验室开放时间：周一至周五", "机器人视觉课程测试", "请保持图像清晰"],
)

save_card(
    "18_clean_mixed_address.png",
    "18 Mixed Address",
    ["收件人：李明", "地址：上海市浦东新区", "Phone: 13800138000"],
)

save_card(
    "19_low_light_invoice.png",
    "19 Low Light Invoice",
    ["发票号码：20260711008", "购买方：视觉算法实验室", "金额：256.80 元"],
    bg=(224, 228, 232),
    fg=(74, 82, 92),
    contrast=0.72,
)

save_table("20_table_chinese_scores.png")

save_card(
    "21_rotated_form.png",
    "21 Rotated Form",
    ["项目名称：OCR文字识别", "负责人：王老师", "状态：测试完成"],
    rotate=-4,
)

save_card(
    "22_blurred_screen.png",
    "22 Slight Blur Screen",
    ["系统提示：识别任务已完成", "输出文件：result.csv", "平均置信度：90"],
    blur=0.8,
)

save_card(
    "23_dense_english.png",
    "23 Dense English",
    ["Computer vision reads image content.", "Tesseract provides OCR results.", "Qt is used for the interface."],
    size=(1120, 560),
)

save_card(
    "24_small_chinese_font.png",
    "24 Small Chinese Font",
    ["小字号文本识别测试", "编号：OCR-2026-07-11", "要求：输出文字和置信度"],
    size=(860, 420),
    fg=(26, 36, 48),
)

save_card(
    "25_symbols_and_numbers.png",
    "25 Symbols and Numbers",
    ["型号：CV-2B-OCR", "温度：25.6 C", "状态：OK / PASS"],
)

save_card(
    "26_long_chinese_paragraph.png",
    "26 Long Chinese Paragraph",
    ["本系统用于从图片中提取文字信息", "测试内容包括中文 英文 数字 表格", "复杂场景下可能出现少量误识别"],
    size=(1080, 560),
)

print(f"generated {len(list(OUT.glob('*.png')))} extra OCR samples in {OUT}")

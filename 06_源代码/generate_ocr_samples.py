from pathlib import Path

from PIL import Image, ImageDraw, ImageEnhance, ImageFont


OUT = Path("test_samples")
OUT.mkdir(parents=True, exist_ok=True)


def font(size=42):
    candidates = [
        "/mnt/c/Windows/Fonts/msyh.ttc",
        "/mnt/c/Windows/Fonts/simhei.ttf",
        "/mnt/c/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/arial.ttf",
    ]
    for path in candidates:
        try:
            return ImageFont.truetype(path, size)
        except OSError:
            pass
    return ImageFont.load_default()


def save_text_image(name, lines, title, size=(900, 420), bg=(255, 255, 255), fg=(28, 38, 52), rotate=0, contrast=1.0):
    img = Image.new("RGB", size, bg)
    draw = ImageDraw.Draw(img)
    draw.text((40, 28), title, fill=(90, 105, 125), font=font(24))
    y = 105
    for line in lines:
        draw.text((70, y), line, fill=fg, font=font(42))
        y += 68
    if contrast != 1.0:
        img = ImageEnhance.Contrast(img).enhance(contrast)
    if rotate:
        img = img.rotate(rotate, expand=True, fillcolor=bg)
    img.save(OUT / name)


save_text_image(
    "01_chinese_simple.png",
    ["机器人视觉学习", "文字识别系统"],
    "01 Chinese text",
)

save_text_image(
    "02_english_simple.png",
    ["OpenCV Tesseract OCR", "Image Text Recognition"],
    "02 English text",
)

save_text_image(
    "03_mixed_cn_en.png",
    ["2B OCR 文字识别", "Python C++ Qt"],
    "03 Mixed Chinese and English",
)

save_text_image(
    "04_numbers_id_like.png",
    ["姓名 张三", "编号 20260711001", "成绩 95"],
    "04 Numbers and fields",
)

save_text_image(
    "05_low_contrast.png",
    ["低对比度文字", "Contrast Test 2026"],
    "05 Low contrast",
    bg=(238, 243, 246),
    fg=(105, 118, 132),
    contrast=0.82,
)

save_text_image(
    "06_rotated_text.png",
    ["轻微旋转文本", "Rotate 5 degrees"],
    "06 Rotated text",
    rotate=5,
)

save_text_image(
    "07_larger_paragraph.png",
    ["OCR 可以从图片中提取文字", "适合票据 表单 标牌 截图", "本例用于课程作业测试"],
    "07 Larger paragraph",
    size=(980, 520),
)

save_text_image(
    "08_receipt_style.png",
    ["商品 A  12.50", "商品 B  35.00", "合计  47.50"],
    "08 Receipt style",
    size=(760, 520),
)

save_text_image(
    "09_table_style.png",
    ["项目    数量    金额", "苹果      3     18.00", "牛奶      2     24.00"],
    "09 Table style",
    size=(900, 520),
)

save_text_image(
    "10_small_font.png",
    ["小字号测试 Small Font Test", "编号 OCR20260711002", "请检查识别效果"],
    "10 Small font",
    size=(820, 420),
)

save_text_image(
    "11_form_style.png",
    ["姓名：李四", "电话：13800138000", "地址：北京市海淀区"],
    "11 Form style",
    size=(860, 460),
)

save_text_image(
    "12_english_paragraph.png",
    ["Computer vision converts images into data.", "OCR extracts readable text from pictures.", "This sample is used for testing."],
    "12 English paragraph",
    size=(1050, 520),
)

save_text_image(
    "13_screen_like.png",
    ["系统状态：运行正常", "任务名称：OCR文字识别", "当前时间：2026-07-11"],
    "13 Screen-like text",
    size=(980, 520),
    bg=(244, 248, 252),
    fg=(30, 45, 65),
)

save_text_image(
    "14_noisy_background.png",
    ["噪声背景测试", "Noise Background 2026"],
    "14 Noisy background",
    size=(860, 430),
    bg=(235, 239, 242),
    fg=(42, 54, 70),
)

save_text_image(
    "15_invoice_like.png",
    ["发票号码 12345678", "购买方 机器人视觉学习", "金额 128.00 元"],
    "15 Invoice-like text",
    size=(900, 480),
)

save_text_image(
    "16_mixed_symbols.png",
    ["型号：A-2026-B", "温度：25.6 C", "状态：OK"],
    "16 Mixed symbols",
    size=(820, 430),
)

print(f"generated {len(list(OUT.glob('*.png')))} OCR samples")

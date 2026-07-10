from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "07_综合检测Demo" / "01_综合测试原图"
OUT.mkdir(parents=True, exist_ok=True)


def font(size=34):
    for path in [
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/arial.ttf",
    ]:
        try:
            return ImageFont.truetype(path, size)
        except OSError:
            continue
    return ImageFont.load_default()


def paste_code(canvas, code_path, box):
    code = Image.open(code_path).convert("RGB")
    code.thumbnail((box[2] - box[0], box[3] - box[1]))
    x = box[0] + ((box[2] - box[0]) - code.width) // 2
    y = box[1] + ((box[3] - box[1]) - code.height) // 2
    canvas.paste(code, (x, y))


def make_sample(name, code_image, lines):
    canvas = Image.new("RGB", (1180, 680), (250, 252, 255))
    draw = ImageDraw.Draw(canvas)
    draw.rounded_rectangle((28, 28, 1152, 652), radius=20, outline=(185, 199, 220), width=3)
    draw.text((56, 50), "综合检测样张：读码 + OCR", fill=(38, 60, 92), font=font(36))
    draw.rounded_rectangle((64, 128, 500, 584), radius=16, fill=(255, 255, 255), outline=(210, 218, 230), width=2)
    paste_code(canvas, code_image, (95, 165, 470, 550))

    draw.rounded_rectangle((540, 128, 1108, 584), radius=16, fill=(255, 255, 255), outline=(210, 218, 230), width=2)
    y = 180
    for line in lines:
        draw.text((585, y), line, fill=(28, 38, 52), font=font(34))
        y += 72

    canvas.save(OUT / name)


code_root = ROOT.parent / "2A读码识别系统" / "01_测试原图"

make_sample(
    "01_qr_id_card_like.png",
    code_root / "01_normal_qr.png",
    [
        "姓名: 杨棋皓",
        "身份证号: 32040420030101999X",
        "电话: 13626256429",
        "地址: 江苏常州",
    ],
)

make_sample(
    "02_code128_invoice_like.png",
    code_root / "11_code128.png",
    [
        "发票号码: 12345678",
        "购买方: 视觉算法实验室",
        "金额: 256.80 元",
        "项目: OCR综合检测",
    ],
)

make_sample(
    "03_mixed_qr_ocr.png",
    code_root / "14_mixed_qr_code128.png",
    [
        "任务: 2A读码 + 2B OCR",
        "状态: 综合检测完成",
        "Phone: 13800138000",
        "Amount: 88.60",
    ],
)

print(f"generated {len(list(OUT.glob('*.png')))} comprehensive samples in {OUT}")

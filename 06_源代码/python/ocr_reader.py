import argparse
import csv
from pathlib import Path

import cv2
import pytesseract


SUPPORTED_EXTENSIONS = {".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff"}


def read_image(image_path: str):
    image = cv2.imread(image_path)
    if image is None:
        raise FileNotFoundError(f"无法读取图片：{image_path}")
    return image


def preprocess_for_ocr(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    scale = 2.0 if gray.shape[1] < 1200 else 1.0
    resized = cv2.resize(gray, None, fx=scale, fy=scale, interpolation=cv2.INTER_CUBIC)
    denoised = cv2.medianBlur(resized, 3)
    _, binary = cv2.threshold(denoised, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    return binary


def recognize_image(image, lang="chi_sim+eng"):
    processed = preprocess_for_ocr(image)
    text = pytesseract.image_to_string(processed, lang=lang)
    data = pytesseract.image_to_data(processed, lang=lang, output_type=pytesseract.Output.DICT)
    confs = [float(c) for c in data["conf"] if c != "-1"]
    confidence = sum(confs) / len(confs) if confs else 0.0
    return " ".join(text.split()), confidence, processed


def iter_images(input_path: str):
    path = Path(input_path)
    if path.is_file():
        yield path
        return
    for image_path in sorted(path.iterdir()):
        if image_path.suffix.lower() in SUPPORTED_EXTENSIONS:
            yield image_path


def main():
    parser = argparse.ArgumentParser(description="2B OCR 文字识别")
    parser.add_argument("input", help="输入图片路径或文件夹")
    parser.add_argument("--output-dir", default="ocr_output", help="预处理图输出文件夹")
    parser.add_argument("--csv", default="ocr_results.csv", help="CSV 输出路径")
    args = parser.parse_args()

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    rows = []
    for image_path in iter_images(args.input):
        image = read_image(str(image_path))
        text, confidence, processed = recognize_image(image)
        output_path = output_dir / f"{image_path.stem}_preprocessed.jpg"
        cv2.imwrite(str(output_path), processed)
        rows.append([image_path.name, text, f"{confidence:.2f}"])
        print(f"图片：{image_path}")
        print(f"识别文本：{text}")
        print(f"平均置信度：{confidence:.2f}")
        print(f"预处理图：{output_path}")

    with open(args.csv, "w", newline="", encoding="utf-8-sig") as file:
        writer = csv.writer(file)
        writer.writerow(["image", "text", "confidence"])
        writer.writerows(rows)

    print(f"总图片数量：{len(rows)}")
    print(f"CSV 结果：{args.csv}")


if __name__ == "__main__":
    main()

# 2B OCR 文字识别系统完成报告

## 一、题目目标

本项目完成 2B OCR 文字识别系统，目标是从图片中识别中文、英文、数字和简单表单文字，并输出识别文本和置信度。

实现内容包括：

- 读取单张图片或文件夹中的多张图片
- 对图片进行灰度化、放大、去噪、二值化预处理
- 使用 Tesseract OCR 识别中文和英文
- 输出识别文本和平均置信度
- 保存预处理后的图片
- 导出 CSV 识别结果
- 提供 Windows 原生 Qt 图形界面
- 提供桌面快捷方式和专用图标
- 整理测试数据、结果、源码和报告

## 二、开发环境

| 项目 | 使用内容 |
|---|---|
| 系统 | Windows |
| 编译环境 | MSYS2 MinGW64 |
| 编译器 | g++ 16.1.0 |
| 构建工具 | CMake + Ninja |
| 图形界面 | Qt 6.11.1 |
| 图像处理 | OpenCV 4.13.0 |
| OCR 引擎 | Tesseract 5.5.2 |
| 图像基础库 | Leptonica 1.87.0 |
| OCR 语言包 | chi_sim、eng、osd |
| 测试图片生成 | Python + Pillow |

工程位置：

```text
C:\Users\86136\Desktop\vision_practice_windows\ocr_reader
```

主要可执行文件：

```text
cpp\build\ocr_qt.exe
cpp\build\ocr_cli.exe
```

桌面快捷方式：

```text
C:\Users\86136\Desktop\2B OCR文字识别系统.lnk
```

## 三、实现过程

### 1. 环境准备

Windows 中已经具备 MSYS2、CMake、g++、OpenCV 和 Qt6。随后补充安装 OCR 必需组件：

- Tesseract
- Leptonica
- 中文语言包 `chi_sim`
- 英文语言包 `eng`

语言包检查结果：

```text
chi_sim
eng
osd
```

### 2. Python 版本

为了快速验证 OCR 流程，先实现 Python 版本。

主要文件：

```text
python\ocr_reader.py
```

功能：

- 使用 OpenCV 读取图片
- 灰度化、放大、去噪、二值化
- 使用 pytesseract 识别文本
- 输出文本和置信度
- 保存预处理图
- 导出 CSV

### 3. C++ 命令行版本

C++ 版本将 OCR 能力封装为 `ocr_core`。

主要文件：

```text
cpp\include\ocr_core.hpp
cpp\src\ocr_core.cpp
cpp\src\cli_main.cpp
```

核心函数：

| 函数 | 作用 |
|---|---|
| `collectImagePaths()` | 收集图片 |
| `readImage()` | 读取图片 |
| `preprocessForOcr()` | OCR 预处理 |
| `recognizeImage()` | 调用 Tesseract 识别 |
| `saveResultsCsv()` | 保存 CSV 结果 |

命令行运行方式：

```bat
cpp\build\ocr_cli.exe test_samples ocr_output_full_test ocr_results_full_test.csv
```

### 4. Qt 图形界面版本

主要文件：

```text
cpp\src\qt_main.cpp
```

界面功能：

- 打开图片
- 预览原图或预处理图
- 识别当前图片
- 显示 OCR 文本
- 显示平均置信度
- 保存识别文本

界面风格沿用 2A 的柔和浅色桌面软件风格：

- 中文界面
- 字体使用 `Microsoft YaHei UI`
- 大窗口布局
- 左侧图像预览
- 右侧操作面板和识别文本
- 背景与卡片对比更清楚

## 四、测试数据

测试图片统一保存在：

```text
test_samples
```

本次共准备 26 张测试图片，覆盖中文、英文、中英混合、数字字段、低对比度、旋转文本、表格、票据样式、截图样式、低光票据、轻微模糊、小字号中文和长段落等情况。

| 编号 | 图片 | 测试内容 |
|---|---|---|
| 01 | `01_chinese_simple.png` | 简单中文 |
| 02 | `02_english_simple.png` | 英文文本 |
| 03 | `03_mixed_cn_en.png` | 中英混合 |
| 04 | `04_numbers_id_like.png` | 姓名、编号、成绩 |
| 05 | `05_low_contrast.png` | 低对比度文本 |
| 06 | `06_rotated_text.png` | 轻微旋转文本 |
| 07 | `07_larger_paragraph.png` | 多行中文段落 |
| 08 | `08_receipt_style.png` | 小票样式 |
| 09 | `09_table_style.png` | 表格样式 |
| 10 | `10_small_font.png` | 小字号文本 |
| 11 | `11_form_style.png` | 表单字段 |
| 12 | `12_english_paragraph.png` | 英文段落 |
| 13 | `13_screen_like.png` | 截图样式文本 |
| 14 | `14_noisy_background.png` | 噪声背景 |
| 15 | `15_invoice_like.png` | 发票样式 |
| 16 | `16_mixed_symbols.png` | 符号、数字、英文混合 |
| 17 | `17_clean_chinese_notice.png` | 清晰中文通知 |
| 18 | `18_clean_mixed_address.png` | 中英文地址与电话 |
| 19 | `19_low_light_invoice.png` | 低光发票样式 |
| 20 | `20_table_chinese_scores.png` | 中文表格字段 |
| 21 | `21_rotated_form.png` | 轻微旋转表单 |
| 22 | `22_blurred_screen.png` | 轻微模糊截图 |
| 23 | `23_dense_english.png` | 密集英文段落 |
| 24 | `24_small_chinese_font.png` | 小字号中文 |
| 25 | `25_symbols_and_numbers.png` | 型号、数字和符号 |
| 26 | `26_long_chinese_paragraph.png` | 较长中文段落 |

## 五、测试结果

测试命令：

```bat
cpp\build\ocr_cli.exe test_samples ocr_output_full_test ocr_results_full_test.csv
```

输出位置：

```text
ocr_output_full_test
ocr_results_full_test.csv
full_test_log.txt
```

测试结果汇总：

| 指标 | 结果 |
|---|---|
| 测试图片数量 | 16 张 |
| 预处理输出图片数量 | 16 张 |
| CSV 结果记录 | 16 条 |
| 最高置信度 | 95 |
| 最低置信度 | 76 |
| C++ 核心测试 | 通过 |
| Windows Qt 编译 | 通过 |

补充测试结果：

| 指标 | 结果 |
|---|---|
| 补充测试图片数量 | 10 张 |
| 补充预处理输出图片数量 | 10 张 |
| 补充 CSV 结果记录 | 10 条 |
| 补充最高置信度 | 95 |
| 补充最低置信度 | 86 |
| 补充平均置信度 | 90.2 |

合并后总结果：

| 指标 | 结果 |
|---|---|
| 总测试图片数量 | 26 张 |
| 总预处理输出图片数量 | 26 张 |
| 总 CSV 结果记录 | 26 条 |

补充测试命令：

```bat
cpp\build\ocr_cli.exe test_samples_more ocr_output_more_test ocr_results_more_test.csv
```

补充测试输出位置：

```text
test_samples_more
ocr_output_more_test
ocr_results_more_test.csv
more_test_log.txt
```

整体结果：

- 英文段落、数字字段、中英混合文本识别效果较好
- 低对比度文本识别效果较好
- 轻微旋转文本可以识别，但有个别误字
- 表格、小票、发票类图片可以提取主要信息，但中文字段可能有少量误识别

典型误识别示例：

| 原意 | OCR 输出现象 |
|---|---|
| 视觉 | 有时识别成“视部”或“视帝” |
| 轻微 | 有时识别成“经微” |
| 商品 A | 小票样式中可能识别成 `fan A` |
| 金额 | 发票样式中可能识别成“全额” |

这些问题属于传统 Tesseract OCR 在中文复杂场景、旋转文本、表格文本中的常见局限。

## 六、项目文件说明

| 路径 | 说明 |
|---|---|
| `python\ocr_reader.py` | Python OCR 版本 |
| `cpp\include\ocr_core.hpp` | C++ OCR 核心接口 |
| `cpp\src\ocr_core.cpp` | C++ OCR 核心实现 |
| `cpp\src\cli_main.cpp` | C++ 命令行入口 |
| `cpp\src\qt_main.cpp` | Qt 图形界面入口 |
| `cpp\tests\test_core.cpp` | C++ 核心测试 |
| `test_samples` | OCR 测试图片 |
| `ocr_output_full_test` | 预处理图片输出 |
| `ocr_results_full_test.csv` | OCR 结果表格 |
| `full_test_log.txt` | 完整测试日志 |
| `启动2B_Qt界面.bat` | 启动 Qt 界面 |
| `运行2B_CLI测试.bat` | 运行命令行测试 |
| `2B_ocr_icon.ico` | 桌面快捷方式图标 |

## 七、使用方法

### 方法一：双击桌面快捷方式

桌面双击：

```text
2B OCR文字识别系统
```

进入界面后：

1. 点击“打开图片”
2. 选择测试图片
3. 点击“识别当前图片”
4. 查看右侧识别文本和置信度
5. 可选择保存文本

### 方法二：运行命令行批量测试

进入工程目录：

```bat
cd C:\Users\86136\Desktop\vision_practice_windows\ocr_reader
```

运行：

```bat
运行2B_CLI测试.bat
```

或手动运行：

```bat
cpp\build\ocr_cli.exe test_samples ocr_output_full_test ocr_results_full_test.csv
```

## 八、完成情况总结

2B OCR 文字识别系统已经完成：

- Python 版本
- C++ 命令行版本
- Qt 图形界面版本
- Windows 原生运行
- 中文和英文 OCR 语言包配置
- 26 组测试图片
- 批量 OCR 测试
- CSV 结果
- 预处理图片输出
- 桌面快捷方式和图标
- 完整报告和源码整理

该系统可以作为课程作业中的 OCR 识别题完整实现。

## 九、选做功能补充说明

根据题目中的 2B 进阶选做要求，本项目已补充完成以下内容：

| 选做项 | 完成情况 | 说明 |
|---|---|---|
| 识别身份证等结构化信息 | 已完成 | 综合检测结果表中增加 `name`、`id_number`、`phone`、`address`、`invoice_number`、`amount` 字段，可从 OCR 文本中提取典型结构化信息 |
| 中英文混合识别 | 已完成 | 原有 26 张测试图中包含中英文混合、英文段落、中文段落、数字字段等样例 |
| 导出结果为 Excel | 已完成 | 新增 `03_结果表格/2B完整OCR识别结果_26张.xlsx`，综合检测结果同时导出 CSV 和 Excel |
| 结合读码 + OCR 做综合检测 | 已完成 | 新增 `07_综合检测Demo`，同一张图片中同时检测二维码/条形码和 OCR 文字 |

### 综合检测 Demo 说明

综合检测 Demo 的流程为：

```text
输入综合样张 -> ZBar 识别二维码/条形码 -> Tesseract OCR 识别文字 -> 提取结构化字段 -> 绘制综合标注图 -> 导出 CSV 与 Excel
```

综合检测 Demo 文件结构：

```text
07_综合检测Demo
├── 01_综合测试原图
├── 02_综合标注输出图
├── 03_综合检测结果表格
└── 综合检测运行日志.txt
```

本次综合检测共准备 3 张样张：

| 图片 | 内容 |
|---|---|
| `01_qr_id_card_like.png` | 二维码 + 姓名、身份证号、电话、地址 |
| `02_code128_invoice_like.png` | Code128 条码 + 发票号、购买方、金额 |
| `03_mixed_qr_ocr.png` | 混合码图 + 中英文 OCR 文本 |

综合检测输出：

| 输出 | 说明 |
|---|---|
| `02_综合标注输出图/*_combined.jpg` | 同时标注读码区域和 OCR 置信度 |
| `03_综合检测结果表格/综合检测结果.csv` | 综合检测 CSV 表格 |
| `03_综合检测结果表格/综合检测结果.xlsx` | 综合检测 Excel 表格 |

测试结果：3 张综合样张均成功检测到至少 1 个二维码或条形码，并输出 OCR 结果和结构化字段。

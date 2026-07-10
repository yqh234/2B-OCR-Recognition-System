#include "ocr_core.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "用法：ocr_cli <图片或文件夹> [输出文件夹] [CSV路径]\n";
        return 1;
    }

    const fs::path inputPath = argv[1];
    const fs::path outputDir = argc >= 3 ? fs::path(argv[2]) : fs::path("ocr_output");
    const fs::path csvPath = argc >= 4 ? fs::path(argv[3]) : fs::path("ocr_results.csv");

    try {
        fs::create_directories(outputDir);
        std::vector<std::pair<std::string, ocr2b::OcrResult>> rows;

        for (const auto& imagePath : ocr2b::collectImagePaths(inputPath)) {
            const cv::Mat image = ocr2b::readImage(imagePath);
            const cv::Mat processed = ocr2b::preprocessForOcr(image);
            const auto result = ocr2b::recognizeImage(image);

            const fs::path outputPath = outputDir / (imagePath.stem().string() + "_preprocessed.jpg");
            cv::imwrite(outputPath.string(), processed);
            rows.push_back({imagePath.filename().string(), result});

            std::cout << "图片：" << imagePath << '\n';
            std::cout << "识别文本：" << result.text << '\n';
            std::cout << "平均置信度：" << result.confidence << '\n';
            std::cout << "预处理图：" << outputPath << "\n\n";
        }

        ocr2b::saveResultsCsv(rows, csvPath);
        std::cout << "总图片数量：" << rows.size() << '\n';
        std::cout << "CSV 结果：" << csvPath << '\n';
    } catch (const std::exception& exc) {
        std::cerr << "错误：" << exc.what() << '\n';
        return 1;
    }

    return 0;
}

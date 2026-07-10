#pragma once

#include <opencv2/opencv.hpp>

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace ocr2b {

struct OcrResult {
    std::string text;
    double confidence = 0.0;
};

std::string normalizeText(const std::string& text);
std::string escapeCsv(const std::string& value);
std::vector<std::filesystem::path> collectImagePaths(const std::filesystem::path& inputPath);
cv::Mat readImage(const std::filesystem::path& imagePath);
cv::Mat preprocessForOcr(const cv::Mat& image);
OcrResult recognizeImage(const cv::Mat& image, const std::string& language = "chi_sim+eng");
void saveResultsCsv(
    const std::vector<std::pair<std::string, OcrResult>>& rows,
    const std::filesystem::path& csvPath
);

}  // namespace ocr2b

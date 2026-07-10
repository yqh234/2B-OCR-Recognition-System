#include "ocr_core.hpp"

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace ocr2b {
namespace {

const std::vector<std::string> kSupportedExtensions = {
    ".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff"
};

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool isSupportedImage(const fs::path& path) {
    const auto ext = toLower(path.extension().string());
    return std::find(kSupportedExtensions.begin(), kSupportedExtensions.end(), ext)
        != kSupportedExtensions.end();
}

}  // namespace

std::string normalizeText(const std::string& text) {
    std::string normalized;
    bool previousSpace = true;
    for (const unsigned char ch : text) {
        if (ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ') {
            if (!previousSpace) {
                normalized.push_back(' ');
                previousSpace = true;
            }
        } else {
            normalized.push_back(static_cast<char>(ch));
            previousSpace = false;
        }
    }
    if (!normalized.empty() && normalized.back() == ' ') {
        normalized.pop_back();
    }
    return normalized;
}

std::string escapeCsv(const std::string& value) {
    const bool needsQuotes = value.find_first_of(",\"\n\r") != std::string::npos;
    if (!needsQuotes) {
        return value;
    }

    std::string escaped = "\"";
    for (const char ch : value) {
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped += ch;
        }
    }
    escaped += "\"";
    return escaped;
}

std::vector<fs::path> collectImagePaths(const fs::path& inputPath) {
    if (!fs::exists(inputPath)) {
        throw std::runtime_error("input path not found: " + inputPath.string());
    }
    if (fs::is_regular_file(inputPath)) {
        return {inputPath};
    }

    std::vector<fs::path> paths;
    for (const auto& entry : fs::directory_iterator(inputPath)) {
        if (entry.is_regular_file() && isSupportedImage(entry.path())) {
            paths.push_back(entry.path());
        }
    }
    std::sort(paths.begin(), paths.end());
    return paths;
}

cv::Mat readImage(const fs::path& imagePath) {
    cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);
    if (image.empty()) {
        throw std::runtime_error("could not read image: " + imagePath.string());
    }
    return image;
}

cv::Mat preprocessForOcr(const cv::Mat& image) {
    if (image.empty()) {
        return {};
    }

    cv::Mat gray;
    if (image.channels() == 1) {
        gray = image.clone();
    } else {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    }

    cv::Mat scaled;
    const double scale = gray.cols < 1200 ? 2.0 : 1.0;
    cv::resize(gray, scaled, cv::Size(), scale, scale, cv::INTER_CUBIC);

    cv::Mat denoised;
    cv::medianBlur(scaled, denoised, 3);

    cv::Mat binary;
    cv::threshold(denoised, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return binary;
}

OcrResult recognizeImage(const cv::Mat& image, const std::string& language) {
    const cv::Mat processed = preprocessForOcr(image);
    if (processed.empty()) {
        return {};
    }

    tesseract::TessBaseAPI tess;
    const char* tessdataPrefix = std::getenv("TESSDATA_PREFIX");
    const char* defaultTessdata = "C:/msys64/mingw64/share/tessdata";
    const char* tessdataPath = tessdataPrefix && tessdataPrefix[0] != '\0'
        ? tessdataPrefix
        : defaultTessdata;

    if (tess.Init(tessdataPath, language.c_str()) != 0) {
        throw std::runtime_error("failed to initialize tesseract language: " + language);
    }

    tess.SetPageSegMode(tesseract::PSM_AUTO);
    tess.SetImage(
        processed.data,
        processed.cols,
        processed.rows,
        processed.channels(),
        static_cast<int>(processed.step)
    );

    char* rawText = tess.GetUTF8Text();
    std::string text = rawText ? rawText : "";
    delete[] rawText;

    const int confidence = tess.MeanTextConf();
    tess.End();

    return {normalizeText(text), static_cast<double>(confidence)};
}

void saveResultsCsv(
    const std::vector<std::pair<std::string, OcrResult>>& rows,
    const fs::path& csvPath
) {
    if (!csvPath.parent_path().empty()) {
        fs::create_directories(csvPath.parent_path());
    }

    std::ofstream file(csvPath, std::ios::trunc);
    if (!file) {
        throw std::runtime_error("could not write csv: " + csvPath.string());
    }

    file << "image,text,confidence\n";
    for (const auto& [imageName, result] : rows) {
        file << escapeCsv(imageName) << ','
             << escapeCsv(result.text) << ','
             << result.confidence << '\n';
    }
}

}  // namespace ocr2b

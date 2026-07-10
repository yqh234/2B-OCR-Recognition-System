#include "ocr_core.hpp"

#include <zbar.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct BarcodeItem {
    std::string data;
    std::string type;
    cv::Rect rect;
    std::vector<cv::Point> polygon;
};

struct StructuredInfo {
    std::string name;
    std::string idNumber;
    std::string phone;
    std::string address;
    std::string invoiceNumber;
    std::string amount;
};

std::string joinValues(const std::vector<std::string>& values, const std::string& sep) {
    std::ostringstream out;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            out << sep;
        }
        out << values[i];
    }
    return out.str();
}

std::string firstRegexMatch(const std::string& text, const std::regex& pattern) {
    std::smatch match;
    if (std::regex_search(text, match, pattern) && match.size() > 0) {
        return match.str(0);
    }
    return "";
}

std::string valueAfterKeyword(const std::string& text, const std::string& keyword) {
    const auto pos = text.find(keyword);
    if (pos == std::string::npos) {
        return "";
    }

    auto start = pos + keyword.size();
    while (start < text.size() && (text[start] == ':' || text[start] == ' ')) {
        ++start;
    }

    auto end = text.find(' ', start);
    if (end == std::string::npos) {
        end = text.size();
    }
    return text.substr(start, end - start);
}

StructuredInfo extractStructuredInfo(const std::string& text) {
    StructuredInfo info;
    info.idNumber = firstRegexMatch(text, std::regex(R"([1-9][0-9]{16}[0-9Xx])"));
    info.phone = firstRegexMatch(text, std::regex(R"(1[3-9][0-9]{9})"));
    info.invoiceNumber = firstRegexMatch(text, std::regex(R"([0-9]{8,12})"));
    info.amount = firstRegexMatch(text, std::regex(R"([0-9]+\.[0-9]{2})"));
    info.name = valueAfterKeyword(text, u8"姓名");
    info.address = valueAfterKeyword(text, u8"地址");
    return info;
}

std::vector<BarcodeItem> detectBarcodes(const cv::Mat& image) {
    cv::Mat gray;
    if (image.channels() == 1) {
        gray = image;
    } else {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    }

    std::vector<cv::Mat> candidates;
    candidates.push_back(gray);

    cv::Mat equalized;
    cv::equalizeHist(gray, equalized);
    candidates.push_back(equalized);

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0);

    cv::Mat binary;
    cv::threshold(blurred, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    candidates.push_back(binary);

    for (const auto& candidate : candidates) {
        zbar::ImageScanner scanner;
        scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
        zbar::Image zbarImage(
            candidate.cols,
            candidate.rows,
            "Y800",
            candidate.data,
            static_cast<unsigned long>(candidate.total())
        );

        scanner.scan(zbarImage);

        std::vector<BarcodeItem> results;
        for (auto symbol = zbarImage.symbol_begin(); symbol != zbarImage.symbol_end(); ++symbol) {
            std::vector<cv::Point> polygon;
            for (int i = 0; i < symbol->get_location_size(); ++i) {
                polygon.emplace_back(symbol->get_location_x(i), symbol->get_location_y(i));
            }

            cv::Rect rect;
            if (!polygon.empty()) {
                rect = cv::boundingRect(polygon);
            }

            results.push_back({
                symbol->get_data(),
                symbol->get_type_name(),
                rect,
                polygon,
            });
        }

        zbarImage.set_data(nullptr, 0);
        if (!results.empty()) {
            return results;
        }
    }

    return {};
}

cv::Mat drawCombinedResult(
    const cv::Mat& image,
    const std::vector<BarcodeItem>& barcodes,
    const ocr2b::OcrResult& ocr
) {
    cv::Mat output = image.clone();

    for (const auto& item : barcodes) {
        if (item.polygon.size() >= 2) {
            for (size_t i = 0; i < item.polygon.size(); ++i) {
                cv::line(
                    output,
                    item.polygon[i],
                    item.polygon[(i + 1) % item.polygon.size()],
                    cv::Scalar(0, 200, 0),
                    3
                );
            }
        } else if (item.rect.area() > 0) {
            cv::rectangle(output, item.rect, cv::Scalar(0, 200, 0), 3);
        }

        cv::putText(
            output,
            item.type + ": " + item.data,
            cv::Point(std::max(item.rect.x, 10), std::max(item.rect.y - 8, 24)),
            cv::FONT_HERSHEY_SIMPLEX,
            0.6,
            cv::Scalar(0, 0, 220),
            2,
            cv::LINE_AA
        );
    }

    const std::string ocrLine = "OCR confidence: " + std::to_string(static_cast<int>(ocr.confidence));
    cv::rectangle(output, cv::Rect(15, 15, 360, 46), cv::Scalar(255, 245, 220), cv::FILLED);
    cv::putText(
        output,
        ocrLine,
        cv::Point(28, 46),
        cv::FONT_HERSHEY_SIMPLEX,
        0.75,
        cv::Scalar(180, 80, 0),
        2,
        cv::LINE_AA
    );

    return output;
}

void writeHeaderIfNeeded(std::ofstream& file, bool append, bool exists) {
    if (!append || !exists) {
        file << "image,barcode_count,barcode_types,barcode_data,ocr_text,ocr_confidence,"
             << "name,id_number,phone,address,invoice_number,amount\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: comprehensive_demo <image-or-folder> [output-dir] [csv-path]\n";
        return 1;
    }

    const fs::path inputPath = argv[1];
    const fs::path outputDir = argc >= 3 ? fs::path(argv[2]) : fs::path("comprehensive_output");
    const fs::path csvPath = argc >= 4 ? fs::path(argv[3]) : fs::path("comprehensive_results.csv");

    try {
        fs::create_directories(outputDir);
        if (!csvPath.parent_path().empty()) {
            fs::create_directories(csvPath.parent_path());
        }

        const bool exists = fs::exists(csvPath);
        std::ofstream csv(csvPath, std::ios::trunc);
        if (!csv) {
            throw std::runtime_error("could not write csv: " + csvPath.string());
        }
        writeHeaderIfNeeded(csv, false, exists);

        int imageCount = 0;
        int totalCodes = 0;

        for (const auto& imagePath : ocr2b::collectImagePaths(inputPath)) {
            const cv::Mat image = ocr2b::readImage(imagePath);
            const auto barcodes = detectBarcodes(image);
            const auto ocr = ocr2b::recognizeImage(image);
            const auto structured = extractStructuredInfo(ocr.text);

            std::vector<std::string> types;
            std::vector<std::string> data;
            for (const auto& item : barcodes) {
                types.push_back(item.type);
                data.push_back(item.data);
            }

            const auto annotated = drawCombinedResult(image, barcodes, ocr);
            const fs::path outputPath = outputDir / (imagePath.stem().string() + "_combined.jpg");
            cv::imwrite(outputPath.string(), annotated);

            csv << ocr2b::escapeCsv(imagePath.filename().string()) << ','
                << barcodes.size() << ','
                << ocr2b::escapeCsv(joinValues(types, "|")) << ','
                << ocr2b::escapeCsv(joinValues(data, "|")) << ','
                << ocr2b::escapeCsv(ocr.text) << ','
                << ocr.confidence << ','
                << ocr2b::escapeCsv(structured.name) << ','
                << ocr2b::escapeCsv(structured.idNumber) << ','
                << ocr2b::escapeCsv(structured.phone) << ','
                << ocr2b::escapeCsv(structured.address) << ','
                << ocr2b::escapeCsv(structured.invoiceNumber) << ','
                << ocr2b::escapeCsv(structured.amount) << '\n';

            ++imageCount;
            totalCodes += static_cast<int>(barcodes.size());

            std::cout << "image: " << imagePath << '\n'
                      << "barcodes: " << barcodes.size() << '\n'
                      << "ocr confidence: " << ocr.confidence << '\n'
                      << "output: " << outputPath << "\n\n";
        }

        std::cout << "total images: " << imageCount << '\n'
                  << "total barcodes: " << totalCodes << '\n'
                  << "csv: " << csvPath << '\n';
    } catch (const std::exception& exc) {
        std::cerr << "error: " << exc.what() << '\n';
        return 1;
    }

    return 0;
}

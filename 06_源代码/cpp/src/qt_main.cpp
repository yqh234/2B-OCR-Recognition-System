#include "ocr_core.hpp"

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSplitter>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <filesystem>

namespace fs = std::filesystem;

QString zh(const char* text) {
    return QString::fromUtf8(text);
}

class ImagePreview : public QLabel {
public:
    explicit ImagePreview(QWidget* parent = nullptr) : QLabel(parent) {
        setAlignment(Qt::AlignCenter);
        setMinimumSize(820, 560);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setText(zh("打开图片后预览 OCR 图像"));
    }

    void setImage(const QImage& image) {
        image_ = image;
        updatePixmap();
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QLabel::resizeEvent(event);
        updatePixmap();
    }

private:
    void updatePixmap() {
        if (image_.isNull()) {
            clear();
            setText(zh("打开图片后预览 OCR 图像"));
            return;
        }
        setPixmap(QPixmap::fromImage(image_).scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    QImage image_;
};

QImage matToQImage(const cv::Mat& mat) {
    if (mat.empty()) {
        return {};
    }
    cv::Mat rgb;
    if (mat.channels() == 1) {
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
    } else {
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    }
    return QImage(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), QImage::Format_RGB888).copy();
}

void addShadow(QWidget* widget, int blur = 26, int yOffset = 8) {
    auto* shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(blur);
    shadow->setOffset(0, yOffset);
    shadow->setColor(QColor(45, 61, 84, 22));
    widget->setGraphicsEffect(shadow);
}

QFrame* statCard(const QString& label, QLabel** valueLabel) {
    auto* card = new QFrame;
    card->setObjectName("StatCard");
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(4);
    *valueLabel = new QLabel("0");
    (*valueLabel)->setObjectName("StatValue");
    auto* labelWidget = new QLabel(label);
    labelWidget->setObjectName("StatLabel");
    layout->addWidget(*valueLabel);
    layout->addWidget(labelWidget);
    return card;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setFont(QFont("Microsoft YaHei UI", 11));
    app.setStyleSheet(R"(
        QMainWindow { background: #e9eef5; }
        QWidget {
            color: #1f2937;
            font-family: "Microsoft YaHei UI", "Microsoft YaHei", "Segoe UI";
        }
        QLabel#TitleLabel { color: #172033; font-size: 28px; font-weight: 700; }
        QLabel#SubtitleLabel { color: #64748b; font-size: 13px; }
        QLabel#SectionTitle { color: #1e293b; font-size: 17px; font-weight: 700; }
        QLabel#StatusBadge {
            background: #e7f7ec;
            border: 1px solid #a9ddb8;
            border-radius: 16px;
            color: #276738;
            padding: 9px 16px;
            font-weight: 600;
        }
        QLabel#FileLabel {
            background: #ffffff;
            border: 1px solid #cbd8e6;
            border-radius: 11px;
            color: #334155;
            padding: 13px 15px;
        }
        QFrame#TopBar, QFrame#PanelCard, QFrame#PreviewCard {
            background: #ffffff;
            border: 1px solid #cfd9e6;
            border-radius: 16px;
        }
        QFrame#StatCard {
            background: #f7fafc;
            border: 1px solid #d5dfeb;
            border-radius: 14px;
        }
        QLabel#StatValue { color: #172033; font-size: 30px; font-weight: 750; }
        QLabel#StatLabel { color: #5f6f84; font-size: 13px; }
        QPushButton {
            background: #4f9f72;
            color: white;
            border: none;
            border-radius: 12px;
            padding: 13px 20px;
            font-weight: 600;
            min-height: 24px;
        }
        QPushButton:hover { background: #438b63; }
        QPushButton:pressed { background: #387753; }
        QPushButton#SecondaryButton {
            background: #edf8f0;
            color: #2f7247;
            border: 1px solid #b6dfc2;
        }
        QPushButton#SecondaryButton:hover { background: #e2f2e8; }
        QPushButton#QuietButton {
            background: #fbfcfe;
            color: #334155;
            border: 1px solid #cbd8e6;
        }
        QPushButton#QuietButton:hover { background: #eef5f0; }
        QPushButton:disabled { background: #dce5ef; color: #8795a7; border: none; }
        QScrollArea { background: #f4f8fb; border: none; }
        QLabel#ImagePreview {
            background: #f8fbfd;
            color: #748398;
            border: 1px dashed #b7c6d6;
            border-radius: 14px;
            font-size: 17px;
            font-weight: 600;
        }
        QPlainTextEdit {
            background: #ffffff;
            border: 1px solid #cfd9e6;
            border-radius: 14px;
            padding: 12px;
            color: #172033;
            font-size: 15px;
            selection-background-color: #d9f2df;
        }
        QSplitter::handle { background: #cbd8e6; width: 3px; }
    )");

    QMainWindow window;
    window.setWindowTitle(zh("2B OCR 文字识别系统"));
    window.resize(1420, 900);

    auto* central = new QWidget(&window);
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(28, 28, 28, 28);
    rootLayout->setSpacing(20);

    auto* topBar = new QFrame;
    topBar->setObjectName("TopBar");
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(26, 20, 26, 20);
    auto* titleBox = new QWidget;
    auto* titleLayout = new QVBoxLayout(titleBox);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(5);
    auto* titleLabel = new QLabel(zh("2B OCR 文字识别系统"));
    titleLabel->setObjectName("TitleLabel");
    auto* subtitleLabel = new QLabel(zh("C++ / OpenCV / Tesseract / Qt 图形界面"));
    subtitleLabel->setObjectName("SubtitleLabel");
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    auto* statusLabel = new QLabel(zh("就绪"));
    statusLabel->setObjectName("StatusBadge");
    topLayout->addWidget(titleBox, 1);
    topLayout->addWidget(statusLabel);
    addShadow(topBar);

    auto* imageLabel = new ImagePreview;
    imageLabel->setObjectName("ImagePreview");
    auto* scrollArea = new QScrollArea;
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(true);
    auto* previewCard = new QFrame;
    previewCard->setObjectName("PreviewCard");
    auto* previewLayout = new QVBoxLayout(previewCard);
    previewLayout->setContentsMargins(22, 22, 22, 22);
    previewLayout->setSpacing(16);
    auto* previewTitle = new QLabel(zh("图像预览"));
    previewTitle->setObjectName("SectionTitle");
    previewLayout->addWidget(previewTitle);
    previewLayout->addWidget(scrollArea);
    addShadow(previewCard);

    auto* sidePanel = new QFrame;
    sidePanel->setObjectName("PanelCard");
    sidePanel->setMinimumWidth(500);
    auto* sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(24, 24, 24, 24);
    sideLayout->setSpacing(18);
    addShadow(sidePanel);

    auto* actionTitle = new QLabel(zh("操作面板"));
    actionTitle->setObjectName("SectionTitle");
    auto* fileLabel = new QLabel(zh("未选择图片"));
    fileLabel->setObjectName("FileLabel");
    fileLabel->setWordWrap(true);
    auto* openButton = new QPushButton(zh("打开图片"));
    auto* recognizeButton = new QPushButton(zh("识别当前图片"));
    auto* saveTextButton = new QPushButton(zh("保存文本"));
    recognizeButton->setObjectName("SecondaryButton");
    saveTextButton->setObjectName("QuietButton");
    recognizeButton->setEnabled(false);
    saveTextButton->setEnabled(false);

    sideLayout->addWidget(actionTitle);
    sideLayout->addWidget(fileLabel);
    sideLayout->addWidget(openButton);
    sideLayout->addWidget(recognizeButton);
    sideLayout->addWidget(saveTextButton);

    QLabel* confidenceLabel = nullptr;
    QLabel* stateLabel = nullptr;
    auto* statRow = new QHBoxLayout;
    statRow->setSpacing(14);
    statRow->addWidget(statCard(zh("置信度"), &confidenceLabel));
    statRow->addWidget(statCard(zh("状态"), &stateLabel));
    stateLabel->setText("-");
    sideLayout->addLayout(statRow);

    auto* resultTitle = new QLabel(zh("识别文本"));
    resultTitle->setObjectName("SectionTitle");
    auto* resultEdit = new QPlainTextEdit;
    resultEdit->setReadOnly(true);
    sideLayout->addWidget(resultTitle);
    sideLayout->addWidget(resultEdit, 1);

    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(previewCard);
    splitter->addWidget(sidePanel);
    splitter->setStretchFactor(0, 4);
    splitter->setStretchFactor(1, 2);

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(splitter, 1);
    window.setCentralWidget(central);

    fs::path currentImagePath;
    cv::Mat currentImage;
    cv::Mat currentPreview;
    ocr2b::OcrResult currentResult;

    auto refreshImage = [&]() {
        imageLabel->setImage(matToQImage(currentPreview.empty() ? currentImage : currentPreview));
    };

    QObject::connect(openButton, &QPushButton::clicked, [&]() {
        const QString fileName = QFileDialog::getOpenFileName(
            &window,
            zh("选择图片"),
            QString(),
            "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)"
        );
        if (fileName.isEmpty()) {
            return;
        }
        try {
            currentImagePath = fileName.toStdString();
            currentImage = ocr2b::readImage(currentImagePath);
            currentPreview.release();
            currentResult = {};
            resultEdit->clear();
            refreshImage();
            recognizeButton->setEnabled(true);
            saveTextButton->setEnabled(false);
            confidenceLabel->setText("0");
            stateLabel->setText(zh("已打开"));
            fileLabel->setText(QFileInfo(fileName).fileName());
            statusLabel->setText(zh("图片已打开"));
        } catch (const std::exception& exc) {
            QMessageBox::critical(&window, zh("打开失败"), exc.what());
        }
    });

    QObject::connect(recognizeButton, &QPushButton::clicked, [&]() {
        if (currentImage.empty()) {
            return;
        }
        try {
            currentPreview = ocr2b::preprocessForOcr(currentImage);
            currentResult = ocr2b::recognizeImage(currentImage);
            resultEdit->setPlainText(QString::fromStdString(currentResult.text));
            confidenceLabel->setText(QString::number(currentResult.confidence, 'f', 0));
            stateLabel->setText(zh("已识别"));
            statusLabel->setText(zh("识别完成"));
            saveTextButton->setEnabled(true);
            refreshImage();
        } catch (const std::exception& exc) {
            QMessageBox::critical(&window, zh("识别失败"), exc.what());
        }
    });

    QObject::connect(saveTextButton, &QPushButton::clicked, [&]() {
        const QString fileName = QFileDialog::getSaveFileName(&window, zh("保存文本"), "ocr_result.txt", "Text (*.txt)");
        if (fileName.isEmpty()) {
            return;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(&window, zh("保存失败"), zh("无法保存文本文件。"));
            return;
        }
        file.write(resultEdit->toPlainText().toUtf8());
        statusLabel->setText(zh("文本已保存"));
    });

    window.show();
    return app.exec();
}

#include "mainwindow.h"

#include "findpath.h"
#include "tools.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    souceImage = QImage("image/input_image.jpg");
    // 检查图片是否成功加载
    if (!souceImage.isNull()) {
        // 将QImage转换为QPixmap，因为QLabel的setPixmap方法需要一个QPixmap对象
        QPixmap pixmap = QPixmap::fromImage(souceImage);
        // 设置标签的图片
        ui->label->setPixmap(pixmap);
    } else {
        // 如果图片加载失败，可以在这里处理错误
        ui->label->setText(tr("无法载入图片"));
    }
    pathFindingVisualizer.moveToThread(&findPathThread);
    initConnect();
    findPathThread.start();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showStep(std::vector<std::tuple<int, int, FindPath::Direction>> path, std::vector<std::vector<std::tuple<int, int, FindPath::FailReason>>> positionFailReason) {
    auto copyImage = souceImage.copy();
    for (auto& [x, y, direction] : path) {
        copyImage.setPixelColor(x, y, QColor::fromRgb(252, 41, 41));
    }
    for (auto& vectorMap : positionFailReason) {
        for (auto& [x, y, reason] : vectorMap) {
            switch (reason) {
                case FindPath::FailReason::notAllow: {
                    copyImage.setPixelColor(x, y, QColor::fromRgb(0, 0, 0));
                    break;
                } case FindPath::FailReason::otherPath: {
                    copyImage.setPixelColor(x, y, QColor::fromRgb(41, 41, 252));
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    ui->label->setPixmap(QPixmap::fromImage(copyImage));
}

void MainWindow::initConnect() {
    connect(this, &MainWindow::debugPathFindingVisualizer, &pathFindingVisualizer, &PathFindingVisualizer::beginFindFromPosition);
    connect(&pathFindingVisualizer, &PathFindingVisualizer::step, this, &MainWindow::showStep);
    connect(ui->label, &ClickableLabel::click, [this](int x, int y) {
        // debugSuperpositionFind(x, y);
        emit debugPathFindingVisualizer(x, y);
    });
}

void MainWindow::debugImage() {
    // 读取图像
    cv::Mat colorImage = cv::imread("image/input_image.jpg");

    if (colorImage.empty()) {
        return;
    }
    // 将彩色图像转换为灰度图像
    cv::Mat grayImage;
    cv::cvtColor(colorImage, grayImage, cv::COLOR_BGR2GRAY);

    // 计算直方图
    int histSize = 256; // 直方图的 bin 数量
    float range[] = { 0, 256 }; // 像素值范围
    const float *histRange = range;
    cv::Mat hist;
    cv::calcHist(&grayImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    // 对一维的 cv::Mat 进行均值滤波
    // cv::Mat blurHist;
    // cv::blur(hist, blurHist, cv::Size(1, 5)); // 使用均值滤波

    // 将直方图归一化到画布上
    cv::Mat normalizeBlurHist;
    cv::normalize(hist, normalizeBlurHist, 0, 1, cv::NORM_MINMAX);

    // 找到直方图中的最大值
    // double maxValue;
    // cv::minMaxLoc(normalizeBlurHist, nullptr, &maxValue, nullptr, nullptr);

    int maxHeight = 500;
    // 创建直方图画布
    cv::Mat histImage(maxHeight, histSize, CV_8UC1, cv::Scalar(0));

    // 绘制直方图
    for (int i = 0; i < histSize; i++) {
        line(histImage,
             cv::Point(i, maxHeight - 1),
             cv::Point(i, maxHeight - normalizeBlurHist.at<float>(0, i) * maxHeight),
             cv::Scalar(255),
             1
        );
    }
    auto positionList = Tools::findPeaks<float>(0, histSize, 0.1f, [&normalizeBlurHist](int position) {
        return normalizeBlurHist.at<float>(0, position);
    });

    size_t value = 0;
    for (size_t i = 0; i < positionList.size() / 2; i++) {
        value += positionList[i * 2 + 1];
    }
    value /= positionList.size() / 2;
    // 二值化处理
    cv::Mat binaryImage;
    cv::threshold(grayImage, binaryImage, value, 255, cv::THRESH_BINARY);

    // 定义结构元素
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    // 开运算
    cv::Mat openedImage;
    cv::morphologyEx(binaryImage, openedImage, cv::MORPH_OPEN, element);

    // 闭运算
    cv::Mat closedImage;
    cv::morphologyEx(binaryImage, closedImage, cv::MORPH_CLOSE, element);

    // 膨胀
    cv::Mat dilatedImage;
    cv::dilate(binaryImage, dilatedImage, element);

    // 腐蚀
    cv::Mat erodedImage;
    cv::erode(binaryImage, erodedImage, element);

    // 连续运算
    cv::Mat morphologicalImage = binaryImage.clone();
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));

    // 进行均值滤波
    cv::Mat blurImage;
    cv::blur(grayImage, blurImage, cv::Size(20, 20));

    for (int num : positionList) {
        qDebug() << num;
    }
}

void MainWindow::debugFind(int x, int y) {
    // 读取图像
    cv::Mat grayImage = cv::imread("image/input_image.jpg", cv::IMREAD_GRAYSCALE);
    if (grayImage.empty()) {
        return;
    }
    // auto& color = colorImage.at<cv::Vec3b>(y, x);
    // qDebug() << "R：" << static_cast<int>(color[0]) << "G：" << static_cast<int>(color[1]) << "B：" << static_cast<int>(color[2]);

    // 进行均值滤波
    cv::Mat blurImage;
    cv::blur(grayImage, blurImage, cv::Size(10, 10));

    auto findPathHelp = FindPath(blurImage.cols, blurImage.rows);
    findPathHelp.start(x, y, [&blurImage](int x, int y) {
        // 定义感兴趣区域的矩形
        cv::Rect roi(std::max(x - 80, 0), std::max(y - 80, 0), std::min(x + 80, blurImage.cols) - std::max(x - 80, 0), std::min(y + 80, blurImage.rows) - std::max(y - 80, 0));
        // 获取感兴趣区域
        cv::Mat roiImage = blurImage(roi);
        // 计算直方图
        int histSize = 256;     // 直方图的 bin 数量
        float range[] = { 0, 256 };     // 像素值范围
        const float *histRange = range;
        cv::Mat hist;
        cv::calcHist(&roiImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);
        // 将直方图归一化
        cv::Mat normalizeHist;
        cv::normalize(hist, normalizeHist, 0, 1, cv::NORM_MINMAX);

        // 对一维的 cv::Mat 进行均值滤波
        // cv::Mat blurHist;
        // cv::blur(normalizeHist, blurHist, cv::Size(1, 5)); // 使用均值滤波
        //// 对一维的 cv::Mat 进行均值滤波
        // cv::blur(blurHist, normalizeHist, cv::Size(1, 5)); // 使用均值滤波
        //// 对一维的 cv::Mat 进行均值滤波
        // cv::blur(normalizeHist, blurHist, cv::Size(1, 5)); // 使用均值滤波

        //// 创建可视化直方图
        // int maxHeight = 500;
        //// 创建直方图画布
        // cv::Mat histImage(maxHeight, histSize, CV_8UC1, cv::Scalar(0));
        //// 绘制直方图
        // for (int i = 0; i < histSize; i++) {
        // line(histImage,
        // cv::Point(i, maxHeight - 1),
        // cv::Point(i, maxHeight - blurHist.at<float>(0, i) * maxHeight),
        // cv::Scalar(255),
        // 1
        // );
        // }
        // 寻找峰值
        auto positionList = Tools::findPeaks<float>(0, histSize, 0.1f, [&normalizeHist](int position) {
            return normalizeHist.at<float>(0, position);
        });
        if (positionList.size() < 3) {
            return false;
        }
        // 寻找鞍点中值
        int value = 0;
        for (size_t i = 0; i < positionList.size() / 2; i++) {
            value += positionList[i * 2 + 1];
        }
        value /= positionList.size() / 2;
        if (blurImage.at<unsigned char>(y, x) > value) {
            return true;
        }
        return false;
    }, 20, 4);

    // 创建初始灰度为 127 的图像
    cv::Mat grayMapImage(grayImage.rows, grayImage.cols, CV_8UC1, cv::Scalar(127));
    findPathHelp.getMapData([&grayMapImage](int x, int y, bool allow) {
        grayMapImage.at<unsigned char>(y, x) = allow ? 255 : 0;
    });
    qDebug() << "计算完毕";
}

void MainWindow::debugFindPreprocessingImage(int x, int y) {
    // 读取图像
    cv::Mat colorImage = cv::imread("image/input_image.jpg");

    if (colorImage.empty()) {
        return;
    }
    // 将彩色图像转换为灰度图像
    cv::Mat grayImage;
    cv::cvtColor(colorImage, grayImage, cv::COLOR_BGR2GRAY);

    // 计算直方图
    int histSize = 256;     // 直方图的 bin 数量
    float range[] = { 0, 256 };     // 像素值范围
    const float *histRange = range;
    cv::Mat hist;
    cv::calcHist(&grayImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    // 对一维的 cv::Mat 进行均值滤波
    // cv::Mat blurHist;
    // cv::blur(hist, blurHist, cv::Size(1, 5)); // 使用均值滤波

    // 将直方图归一化到画布上
    cv::Mat normalizeBlurHist;
    cv::normalize(hist, normalizeBlurHist, 0, 1, cv::NORM_MINMAX);

    // 找到直方图中的最大值
    // double maxValue;
    // cv::minMaxLoc(normalizeBlurHist, nullptr, &maxValue, nullptr, nullptr);

    int maxHeight = 500;
    // 创建直方图画布
    cv::Mat histImage(maxHeight, histSize, CV_8UC1, cv::Scalar(0));

    // 绘制直方图
    for (int i = 0; i < histSize; i++) {
        line(histImage,
             cv::Point(i, maxHeight - 1),
             cv::Point(i, maxHeight - normalizeBlurHist.at<float>(0, i) * maxHeight),
             cv::Scalar(255),
             1
        );
    }
    auto positionList = Tools::findPeaks<float>(0, histSize, 0.1f, [&normalizeBlurHist](int position) {
        return normalizeBlurHist.at<float>(0, position);
    });

    size_t value = 0;
    for (size_t i = 0; i < positionList.size() / 2; i++) {
        value += positionList[i * 2 + 1];
    }
    value /= positionList.size() / 2;
    // 二值化处理
    cv::Mat binaryImage;
    cv::threshold(grayImage, binaryImage, value, 255, cv::THRESH_BINARY);

    // 连续运算
    cv::Mat morphologicalImage = binaryImage.clone();
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    auto findPathHelp = FindPath(morphologicalImage.cols, morphologicalImage.rows);
    findPathHelp.start(x, y, [&morphologicalImage](int x, int y) {
        if (morphologicalImage.at<unsigned char>(y, x) > 127) {
            return true;
        }
        return false;
    }, 20, 3);

    // 创建初始灰度为 127 的图像
    cv::Mat grayMapImage(grayImage.rows, grayImage.cols, CV_8UC1, cv::Scalar(127));
    findPathHelp.getMapData([&grayMapImage](int x, int y, bool allow) {
        grayMapImage.at<unsigned char>(y, x) = allow ? 255 : 0;
    });
    qDebug() << "计算完毕";
}

void MainWindow::debugSuperpositionFind(int x, int y) {
    static bool init = false;
    // 读取图像
    static cv::Mat grayImage = cv::imread("image/input_image.jpg", cv::IMREAD_GRAYSCALE);
    if (grayImage.empty()) {
        return;
    }
    // auto& color = colorImage.at<cv::Vec3b>(y, x);
    // qDebug() << "R：" << static_cast<int>(color[0]) << "G：" << static_cast<int>(color[1]) << "B：" << static_cast<int>(color[2]);

    // 进行均值滤波
    static cv::Mat blurImage;
    if (!init) {
        cv::blur(grayImage, blurImage, cv::Size(10, 10));
        // 连续运算
        cv::Mat morphologicalImage = blurImage.clone();
        cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
        cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
        cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
        cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
        cv::morphologyEx(morphologicalImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
        cv::morphologyEx(morphologicalImage, blurImage, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    }
    init = true;
    static auto findPathHelp = FindPath(blurImage.cols, blurImage.rows);
    findPathHelp.start(x, y, [](int x, int y) {
        // 定义感兴趣区域的矩形
        cv::Rect roi(std::max(x - 80, 0), std::max(y - 80, 0), std::min(x + 80, blurImage.cols) - std::max(x - 80, 0), std::min(y + 80, blurImage.rows) - std::max(y - 80, 0));
        // 获取感兴趣区域
        cv::Mat roiImage = blurImage(roi);
        // 计算直方图
        int histSize = 256;     // 直方图的 bin 数量
        float range[] = { 0, 256 };     // 像素值范围
        const float *histRange = range;
        cv::Mat hist;
        cv::calcHist(&roiImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);
        // 将直方图归一化
        cv::Mat normalizeHist;
        cv::normalize(hist, normalizeHist, 0, 1, cv::NORM_MINMAX);

        // 对一维的 cv::Mat 进行均值滤波
        // cv::Mat blurHist;
        // cv::blur(normalizeHist, blurHist, cv::Size(1, 5)); // 使用均值滤波
        //// 对一维的 cv::Mat 进行均值滤波
        // cv::blur(blurHist, normalizeHist, cv::Size(1, 5)); // 使用均值滤波
        //// 对一维的 cv::Mat 进行均值滤波
        // cv::blur(normalizeHist, blurHist, cv::Size(1, 5)); // 使用均值滤波

        //// 创建可视化直方图
        // int maxHeight = 500;
        //// 创建直方图画布
        // cv::Mat histImage(maxHeight, histSize, CV_8UC1, cv::Scalar(0));
        //// 绘制直方图
        // for (int i = 0; i < histSize; i++) {
        // line(histImage,
        // cv::Point(i, maxHeight - 1),
        // cv::Point(i, maxHeight - blurHist.at<float>(0, i) * maxHeight),
        // cv::Scalar(255),
        // 1
        // );
        // }
        // 寻找峰值
        auto positionList = Tools::findPeaks<float>(0, histSize, 0.1f, [&normalizeHist](int position) {
            return normalizeHist.at<float>(0, position);
        });
        if (positionList.size() < 3) {
            return false;
        }
        // 寻找鞍点中值
        int value = 0;
        for (size_t i = 0; i < positionList.size() / 2; i++) {
            value += positionList[i * 2 + 1];
        }
        value /= positionList.size() / 2;
        if (blurImage.at<unsigned char>(y, x) > value) {
            return true;
        }
        return false;
    }, 20, 4);

    // 创建初始灰度为 127 的图像
    cv::Mat grayMapImage(grayImage.rows, grayImage.cols, CV_8UC1, cv::Scalar(127));
    findPathHelp.getMapData([&grayMapImage](int x, int y, bool allow) {
        grayMapImage.at<unsigned char>(y, x) = allow ? 255 : 0;
    });
    qDebug() << "计算完毕";
}

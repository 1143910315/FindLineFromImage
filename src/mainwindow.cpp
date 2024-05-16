#include "mainwindow.h"
#include "tools.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QImage image("image/input_image.jpg");
    // 检查图片是否成功加载
    if (!image.isNull()) {
        // 将QImage转换为QPixmap，因为QLabel的setPixmap方法需要一个QPixmap对象
        QPixmap pixmap = QPixmap::fromImage(image);
        // 设置标签的图片
        ui->label->setPixmap(pixmap);
    } else {
        // 如果图片加载失败，可以在这里处理错误
        ui->label->setText(tr("无法载入图片"));
    }
    // 读取图像
    cv::Mat colorImage = cv::imread("image/input_image.jpg");

    if (image.isNull()) {
        return;
    }
    // 将彩色图像转换为灰度图像
    cv::Mat grayImage;
    cv::cvtColor(colorImage, grayImage, cv::COLOR_BGR2GRAY);

    // 计算直方图
    int histSize = 256; // 直方图的 bin 数量
    float range[] = { 0, 256 }; // 像素值范围
    const float *histRange = range;
    bool uniform = true, accumulate = false;
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
    for (int num : positionList) {
        qDebug() << num;
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

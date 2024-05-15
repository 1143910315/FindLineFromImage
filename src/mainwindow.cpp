#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>

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

    // 找到直方图中的最大值
    double maxValue;
    cv::minMaxLoc(hist, nullptr, &maxValue, nullptr, nullptr);

    int maxHeight = 500;
    // 创建直方图画布
    cv::Mat histImage(maxHeight, histSize, CV_8UC1, cv::Scalar(0));

    // 将直方图归一化到画布上
    // normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // 绘制直方图
    for (int i = 0; i < histSize; i++) {
        line(histImage,
             cv::Point(i, maxHeight - 1),
             cv::Point(i, maxHeight - hist.at<float>(0, i) * maxHeight / maxValue),
             cv::Scalar(255), 1);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

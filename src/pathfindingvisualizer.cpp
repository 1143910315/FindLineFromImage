#include "pathfindingvisualizer.h"

#include "tools.h"
#include <QDebug>

PathFindingVisualizer::PathFindingVisualizer(QObject *parent)
    : QThread(parent) {
    cv::Mat grayImage = cv::imread("image/input_image.jpg", cv::IMREAD_GRAYSCALE);
    if (grayImage.empty()) {
        return;
    }
    cv::Mat morphologicalImage;
    cv::morphologyEx(grayImage, morphologicalImage, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::Mat morphologicalImage1;
    cv::morphologyEx(morphologicalImage, morphologicalImage1, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::Mat morphologicalImage2;
    cv::morphologyEx(morphologicalImage1, morphologicalImage2, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::Mat morphologicalImage3;
    cv::morphologyEx(morphologicalImage2, morphologicalImage3, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::Mat morphologicalImage4;
    cv::morphologyEx(morphologicalImage3, morphologicalImage4, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::Mat morphologicalImage5;
    cv::morphologyEx(morphologicalImage4, morphologicalImage5, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::Mat morphologicalImage6;
    cv::morphologyEx(morphologicalImage5, morphologicalImage6, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(6, 6)));
    cv::Mat morphologicalImage7;
    cv::morphologyEx(morphologicalImage6, morphologicalImage7, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(6, 6)));
    cv::Mat blurImage;
    cv::blur(morphologicalImage7, blurImage, cv::Size(6, 6));
    blurImage.copyTo(cvImage);
    findPath.init(blurImage.cols, blurImage.rows);
}

PathFindingVisualizer::~PathFindingVisualizer() {
}

void PathFindingVisualizer::beginFindFromPosition(int x, int y) {
    this->x = x;
    this->y = y;
    start();
}

void PathFindingVisualizer::run() {
    positionFailReason.push_back({});
    findPath.startVisualization(x, y, 20, 4, [this](int x, int y) {
        // 定义感兴趣区域的矩形
        cv::Rect roi(std::max(x - 80, 0), std::max(y - 80, 0), std::min(x + 80, cvImage.cols) - std::max(x - 80, 0), std::min(y + 80, cvImage.rows) - std::max(y - 80, 0));
        // 获取感兴趣区域
        cv::Mat roiImage = cvImage(roi);

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
        if (cvImage.at<unsigned char>(y, x) > value) {
            return true;
        }
        return false;
    }, [this](int x, int y, FindPath::FailReason failReason) {
        positionFailReason[positionFailReason.size() - 1].push_back(std::make_tuple(x, y, failReason));
    }, [this]() {
        auto& path = findPath.getPath();
        std::vector<std::tuple<int, int, FindPath::Direction>> pathList;
        auto nowX = x;
        auto nowY = y;
        for (auto& [direction, distance] : path) {
            switch (direction) {
                case FindPath::Direction::up: {
                    for (size_t i = 0; i < distance; i++) {
                        nowY--;
                        pathList.push_back(std::make_tuple(nowX, nowY, direction));
                    }
                    break;
                }
                case FindPath::Direction::down: {
                    for (size_t i = 0; i < distance; i++) {
                        nowY++;
                        pathList.push_back(std::make_tuple(nowX, nowY, direction));
                    }
                    break;
                }
                case FindPath::Direction::left: {
                    for (size_t i = 0; i < distance; i++) {
                        nowX--;
                        pathList.push_back(std::make_tuple(nowX, nowY, direction));
                    }
                    break;
                }
                case FindPath::Direction::right: {
                    for (size_t i = 0; i < distance; i++) {
                        nowX++;
                        pathList.push_back(std::make_tuple(nowX, nowY, direction));
                    }
                    break;
                }
                default: {
                    pathList.push_back(std::make_tuple(nowX, nowY, direction));
                    break;
                }
            }
        }
        emit step(pathList, positionFailReason);
    });

    // 创建初始灰度为 127 的图像
    cv::Mat grayMapImage(cvImage.rows, cvImage.cols, CV_8UC1, cv::Scalar(127));
    findPath.getMapData([&grayMapImage](int x, int y, bool allow) {
        grayMapImage.at<unsigned char>(y, x) = allow ? 255 : 0;
    });
    qDebug() << "计算完毕";
}

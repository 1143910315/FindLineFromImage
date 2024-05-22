#pragma once

#include "findpath.h"
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QThread>

class PathFindingVisualizer : protected QThread {
    Q_OBJECT

public:
    PathFindingVisualizer(QObject *parent = nullptr);
    ~PathFindingVisualizer();
    void beginFindFromPosition(int x, int y);
protected:
    // 重写 run 方法，在其中执行具体的工作
    void run() override;
signals:
    // 定义一个信号，表示工作完成
    void step(std::vector<std::tuple<int, int, FindPath::Direction>> path, std::vector<std::vector<std::tuple<int, int, FindPath::FailReason>>> positionFailReason);
private:
    cv::Mat cvImage;
    int x = 0;
    int y = 0;
    FindPath findPath;
    std::vector<std::vector<std::tuple<int, int, FindPath::FailReason>>> positionFailReason;
};

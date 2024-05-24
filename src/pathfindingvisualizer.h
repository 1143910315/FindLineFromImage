#pragma once

#include "findpath.h"
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QThread>

class PathFindingVisualizer : public QObject {
    Q_OBJECT

public:
    PathFindingVisualizer(QObject *parent = nullptr);
    ~PathFindingVisualizer();
signals:
    // 定义一个信号，表示工作完成
    void step(std::vector<std::tuple<int, int, FindPath::Direction>> path, std::vector<std::vector<std::tuple<int, int, FindPath::FailReason>>> positionFailReason);
public slots:
    void beginFindFromPosition(int beginX, int beginY);
private:
    cv::Mat cvImage;
    FindPath findPath;
    std::vector<std::vector<std::tuple<int, int, FindPath::FailReason>>> positionFailReason;
};

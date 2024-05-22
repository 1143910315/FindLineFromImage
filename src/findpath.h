#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

class FindPath {
public:
    enum class Direction:char {
        none, up, down, left, right
    };
    // 定义一个名为MyEnum的枚举类
    enum class FailReason {
        none,
        notAllow,
        otherPath
    };
    FindPath();
    FindPath(int width, int height);
    ~FindPath();
    void init(int width, int height);
    void start(int startX, int startY, std::function<bool(int x, int y)> getMap, int pathPositionDistance, int stepDistance);
    void startVisualization(int startX, int startY, int pathPositionDistance, int stepDistance, std::function<bool(int x, int y)> getMap, std::function<void(int newX, int newY, FailReason failReason)> addFailReasonFunction, std::function<void()> stepFunction);
    void getMapData(std::function<void(int x, int y, bool allow)> mapData);
    const std::vector<std::tuple<Direction, int>>& getPath();
private:
    const int width;
    const int height;
    std::vector<std::vector<std::optional<bool>>> map;
    std::vector<std::vector<int>> mapPositionToPath;
    std::vector<std::tuple<int, int, std::set<int>>> pathPosition;
    std::vector<std::tuple<Direction, int>> path;
    bool needClear = false;
    // 计算曼哈顿距离
    static int manhattanDistance(int x1, int y1, int x2, int y2);
};

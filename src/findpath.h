#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

class FindPath {
    enum class Direction:char {
        none, up, down, left, right
    };
public:
    FindPath(int width, int height);
    ~FindPath();
    void start(int startX, int startY, std::function<bool(int x, int y)> getMap, int pathPositionDistance);
    void getMapData(std::function<void(int x, int y, bool allow)> mapData);
private:
    const int width;
    const int height;
    std::vector<std::vector<std::optional<bool>>> map;
    std::vector<std::vector<int>> mapPositionToPath;
    std::vector<std::tuple<int, int, std::set<int>>> pathPosition;
    std::vector<Direction> path;
    bool needClear = false;
    // 计算曼哈顿距离
    static int manhattanDistance(int x1, int y1, int x2, int y2);
};

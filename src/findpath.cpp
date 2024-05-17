#include "findpath.h"
#include <cmath>

FindPath::FindPath(int width, int height) : width(width), height(height) {
    map = std::vector(width, std::vector(height, std::optional<bool>()));
    mapPositionToPath = std::vector(width, std::vector(height, -1));
}

FindPath::~FindPath() {
}

void FindPath::start(int startX, int startY, std::function<bool(int x, int y)> getMap, int pathPositionDistance) {
    if (needClear) {
        path.clear();
        pathPosition.clear();
        for (auto& v : map) {
            for (auto& i : v) {
                i.reset();
            }
        }
        for (auto& v : mapPositionToPath) {
            for (auto& i : v) {
                i = -1;
            }
        }
    }
    needClear = true;
    path.push_back(Direction::none);
    pathPosition.push_back(std::make_tuple(startX, startY, std::set<int>()));
    mapPositionToPath[startX][startY] = 0;
    std::tuple<int, int> nowPosition = std::make_tuple(startX, startY);
    while (!path.empty()) {
        auto& [nowX, nowY] = nowPosition;
        int newX, newY;
        Direction direction;
        auto pathId = mapPositionToPath[nowX][nowY];
        auto& [pathPositionX, pathPositionY, link] = pathPosition[pathId];
        newX = nowX;
        newY = nowY - 1;
        direction = Direction::up;
        if (newY >= 0 && !map[newX][newY].has_value()) {
            auto allow = getMap(newX, newY);
            map[newX][newY] = allow;
            if (allow) {
                if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                    auto newPathId = mapPositionToPath[newX][newY];
                    if (newPathId < 0) {
                        link.insert(pathPosition.size());
                        mapPositionToPath[newX][newY] = pathPosition.size();
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                    } else {
                        link.insert(newPathId);
                        std::get<2>(pathPosition[newPathId]).insert(pathId);
                    }
                } else {
                    nowPosition = std::make_tuple(newX, newY);
                    path.push_back(direction);
                    mapPositionToPath[newX][newY] = pathId;
                    continue;
                }
            }
        }
        newX = nowX + 1;
        newY = nowY;
        direction = Direction::right;
        if (newX < width && !map[newX][newY].has_value()) {
            auto allow = getMap(newX, newY);
            map[newX][newY] = allow;
            if (allow) {
                if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                    auto newPathId = mapPositionToPath[newX][newY];
                    if (newPathId < 0) {
                        link.insert(pathPosition.size());
                        mapPositionToPath[newX][newY] = pathPosition.size();
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                    } else {
                        link.insert(newPathId);
                        std::get<2>(pathPosition[newPathId]).insert(pathId);
                    }
                } else {
                    nowPosition = std::make_tuple(newX, newY);
                    path.push_back(direction);
                    mapPositionToPath[newX][newY] = pathId;
                    continue;
                }
            }
        }
        newX = nowX;
        newY = nowY + 1;
        direction = Direction::down;
        if (newY < height && !map[newX][newY].has_value()) {
            auto allow = getMap(newX, newY);
            map[newX][newY] = allow;
            if (allow) {
                if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                    auto newPathId = mapPositionToPath[newX][newY];
                    if (newPathId < 0) {
                        link.insert(pathPosition.size());
                        mapPositionToPath[newX][newY] = pathPosition.size();
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                    } else {
                        link.insert(newPathId);
                        std::get<2>(pathPosition[newPathId]).insert(pathId);
                    }
                } else {
                    nowPosition = std::make_tuple(newX, newY);
                    path.push_back(direction);
                    mapPositionToPath[newX][newY] = pathId;
                    continue;
                }
            }
        }
        newX = nowX - 1;
        newY = nowY;
        direction = Direction::left;
        if (newX >= 0 && !map[newX][newY].has_value()) {
            auto allow = getMap(newX, newY);
            map[newX][newY] = allow;
            if (allow) {
                if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                    auto newPathId = mapPositionToPath[newX][newY];
                    if (newPathId < 0) {
                        link.insert(pathPosition.size());
                        mapPositionToPath[newX][newY] = pathPosition.size();
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                    } else {
                        link.insert(newPathId);
                        std::get<2>(pathPosition[newPathId]).insert(pathId);
                    }
                } else {
                    nowPosition = std::make_tuple(newX, newY);
                    path.push_back(direction);
                    mapPositionToPath[newX][newY] = pathId;
                    continue;
                }
            }
        }
        switch (path.back()) {
            case Direction::up: {
                nowPosition = std::make_tuple(nowX, nowY + 1);
                break;
            }
            case Direction::down: {
                nowPosition = std::make_tuple(nowX, nowY - 1);
                break;
            }
            case Direction::left: {
                nowPosition = std::make_tuple(nowX, nowY + 1);
                break;
            }
            case Direction::right: {
                nowPosition = std::make_tuple(nowX - 1, nowY);
                break;
            }
            default: {
                break;
            }
        }
        path.pop_back();
    }
}

int FindPath::manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

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
    std::vector<std::tuple<int, int>> manhattanPosition;
    for (int dx = 1; dx <= pathPositionDistance; ++dx) {
        for (int dy = 1; dy <= pathPositionDistance; ++dy) {
            // 如果曼哈顿距离在范围内，则将当前点添加到结果中
            if (dx + dy <= pathPositionDistance) {
                manhattanPosition.push_back(std::make_tuple(dx, dy));
                manhattanPosition.push_back(std::make_tuple(-dx, dy));
                manhattanPosition.push_back(std::make_tuple(dx, -dy));
                manhattanPosition.push_back(std::make_tuple(-dx, -dy));
            }
        }
    }
    for (int dx = 1; dx <= pathPositionDistance; ++dx) {
        // 曼哈顿距离在范围内，则将当前点添加到结果中
        manhattanPosition.push_back(std::make_tuple(dx, 0));
        manhattanPosition.push_back(std::make_tuple(-dx, 0));
    }
    for (int dy = 1; dy <= pathPositionDistance; ++dy) {
        // 曼哈顿距离在范围内，则将当前点添加到结果中
        manhattanPosition.push_back(std::make_tuple(0, dy));
        manhattanPosition.push_back(std::make_tuple(0, -dy));
    }
    manhattanPosition.push_back(std::make_tuple(0, 0));
    path.push_back(Direction::none);
    pathPosition.push_back(std::make_tuple(startX, startY, std::set<int>()));
    mapPositionToPath[startX][startY] = 0;
    auto nowPosition = std::make_tuple(startX, startY);
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
                        for (auto& [manhattanDx, manhattanDy] : manhattanPosition) {
                            int manhattanX = newX + manhattanDx;
                            int manhattanY = newY + manhattanDy;
                            if (manhattanX >= 0 && manhattanX < width && manhattanY >= 0 && manhattanY < height) {
                                mapPositionToPath[manhattanX][manhattanY] = pathPosition.size();
                            }
                        }
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(direction);
                        continue;
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
                        for (auto& [manhattanDx, manhattanDy] : manhattanPosition) {
                            int manhattanX = newX + manhattanDx;
                            int manhattanY = newY + manhattanDy;
                            if (manhattanX >= 0 && manhattanX < width && manhattanY >= 0 && manhattanY < height) {
                                mapPositionToPath[manhattanX][manhattanY] = pathPosition.size();
                            }
                        }
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(direction);
                        continue;
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
                        for (auto& [manhattanDx, manhattanDy] : manhattanPosition) {
                            int manhattanX = newX + manhattanDx;
                            int manhattanY = newY + manhattanDy;
                            if (manhattanX >= 0 && manhattanX < width && manhattanY >= 0 && manhattanY < height) {
                                mapPositionToPath[manhattanX][manhattanY] = pathPosition.size();
                            }
                        }
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(direction);
                        continue;
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
                        for (auto& [manhattanDx, manhattanDy] : manhattanPosition) {
                            int manhattanX = newX + manhattanDx;
                            int manhattanY = newY + manhattanDy;
                            if (manhattanX >= 0 && manhattanX < width && manhattanY >= 0 && manhattanY < height) {
                                mapPositionToPath[manhattanX][manhattanY] = pathPosition.size();
                            }
                        }
                        pathPosition.push_back(std::make_tuple(newX, newY, std::set<int>({ pathId })));
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(direction);
                        continue;
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
                nowPosition = std::make_tuple(nowX + 1, nowY);
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

void FindPath::getMapData(std::function<void(int x, int y, bool allow)> mapData) {
    for (size_t i = 0; i < map.size(); i++) {
        auto& vMap = map[i];
        for (size_t j = 0; j < vMap.size(); j++) {
            auto& allow = vMap[j];
            if (allow.has_value()) {
                mapData(i, j, allow.value());
            }
        }
    }
}

int FindPath::manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

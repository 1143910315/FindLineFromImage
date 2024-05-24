#include "findpath.h"
#include <cmath>

FindPath::FindPath() : width(0), height(0) {
}

FindPath::FindPath(int width, int height) : width(width), height(height) {
    map = std::vector(width, std::vector(height, std::optional<bool>()));
    mapPositionToPath = std::vector(width, std::vector(height, -1));
}

FindPath::~FindPath() {
}

void FindPath::init(int width, int height) {
    this->width = width;
    this->height = height;
    map = std::vector(width, std::vector(height, std::optional<bool>()));
    mapPositionToPath = std::vector(width, std::vector(height, -1));
    needClear = false;
}

void FindPath::start(int startX, int startY, std::function<bool(int x, int y)> getMap, int pathPositionDistance, int stepDistance) {
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
    path.push_back(std::make_tuple(Direction::none, 0));
    pathPosition.push_back(std::make_tuple(startX, startY, std::set<int>()));
    mapPositionToPath[startX][startY] = 0;
    auto nowPosition = std::make_tuple(startX, startY);
    while (!path.empty()) {
        auto pathSize = path.size();
        auto& [nowX, nowY] = nowPosition;
        int newX, newY;
        Direction direction;
        auto pathId = mapPositionToPath[nowX][nowY];
        auto& [pathPositionX, pathPositionY, link] = pathPosition[pathId];
        auto tempStep = stepDistance;
        while (tempStep > 0) {
            newX = nowX;
            newY = nowY - tempStep;
            direction = Direction::up;
            if (newY >= 0 && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int j = nowY; j >= newY; j--) {
                    map[newX][j] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int j = nowY - 1; j >= newY && newPathId < 0; j--) {
                            newPathId = mapPositionToPath[newX][j];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                }
            }
            newX = nowX + tempStep;
            newY = nowY;
            direction = Direction::right;
            if (newX < width && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int i = nowX; i <= newX; i++) {
                    map[i][newY] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int i = nowX + 1; i <= newX && newPathId < 0; i++) {
                            newPathId = mapPositionToPath[i][nowY];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                }
            }
            newX = nowX;
            newY = nowY + tempStep;
            direction = Direction::down;
            if (newY < height && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int j = nowY; j <= newY; j++) {
                    map[newX][j] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int j = nowY + 1; j <= newY && newPathId < 0; j++) {
                            newPathId = mapPositionToPath[newX][j];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                }
            }
            newX = nowX - tempStep;
            newY = nowY;
            direction = Direction::left;
            if (newX >= 0 && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int i = nowX; i >= newX; i--) {
                    map[i][newY] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int i = nowX; i >= newX && newPathId < 0; i--) {
                            newPathId = mapPositionToPath[i][newY];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                }
            }
            tempStep--;
        }
        if (pathSize == path.size()) {
            auto& [direction, distance] = path.back();
            switch (direction) {
                case Direction::up: {
                    nowPosition = std::make_tuple(nowX, nowY + distance);
                    break;
                }
                case Direction::down: {
                    nowPosition = std::make_tuple(nowX, nowY - distance);
                    break;
                }
                case Direction::left: {
                    nowPosition = std::make_tuple(nowX + distance, nowY);
                    break;
                }
                case Direction::right: {
                    nowPosition = std::make_tuple(nowX - distance, nowY);
                    break;
                }
                default: {
                    break;
                }
            }
            path.pop_back();
        }
    }
}

void FindPath::startVisualization(int startX, int startY, int pathPositionDistance, int stepDistance, std::function<bool(int x, int y)> getMap, std::function<void(int newX, int newY, FailReason failReason)> addFailReasonFunction, std::function<void()> stepFunction) {
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
    path.push_back(std::make_tuple(Direction::none, 0));
    pathPosition.push_back(std::make_tuple(startX, startY, std::set<int>()));
    mapPositionToPath[startX][startY] = 0;
    auto nowPosition = std::make_tuple(startX, startY);
    while (!path.empty()) {
        auto pathSize = path.size();
        auto& [nowX, nowY] = nowPosition;
        int newX, newY;
        Direction direction;
        auto pathId = mapPositionToPath[nowX][nowY];
        auto& [pathPositionX, pathPositionY, link] = pathPosition[pathId];
        auto tempStep = stepDistance;
        while (tempStep > 0) {
            newX = nowX;
            newY = nowY - tempStep;
            direction = Direction::up;
            if (newY >= 0 && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int j = nowY; j >= newY; j--) {
                    map[newX][j] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int j = nowY - 1; j >= newY && newPathId < 0; j--) {
                            newPathId = mapPositionToPath[newX][j];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                            addFailReasonFunction(newX, newY, FailReason::otherPath);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                } else {
                    addFailReasonFunction(newX, newY, FailReason::notAllow);
                }
            }
            newX = nowX + tempStep;
            newY = nowY;
            direction = Direction::right;
            if (newX < width && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int i = nowX; i <= newX; i++) {
                    map[i][newY] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int i = nowX + 1; i <= newX && newPathId < 0; i++) {
                            newPathId = mapPositionToPath[i][nowY];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                            addFailReasonFunction(newX, newY, FailReason::otherPath);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                } else {
                    addFailReasonFunction(newX, newY, FailReason::notAllow);
                }
            }
            newX = nowX;
            newY = nowY + tempStep;
            direction = Direction::down;
            if (newY < height && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int j = nowY; j <= newY; j++) {
                    map[newX][j] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int j = nowY + 1; j <= newY && newPathId < 0; j++) {
                            newPathId = mapPositionToPath[newX][j];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                            addFailReasonFunction(newX, newY, FailReason::otherPath);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                } else {
                    addFailReasonFunction(newX, newY, FailReason::notAllow);
                }
            }
            newX = nowX - tempStep;
            newY = nowY;
            direction = Direction::left;
            if (newX >= 0 && !map[newX][newY].has_value()) {
                auto allow = getMap(newX, newY);
                for (int i = nowX; i >= newX; i--) {
                    map[i][newY] = allow;
                }
                if (allow) {
                    if (manhattanDistance(newX, newY, pathPositionX, pathPositionY) > pathPositionDistance) {
                        auto newPathId = -1;
                        for (int i = nowX; i >= newX && newPathId < 0; i--) {
                            newPathId = mapPositionToPath[i][newY];
                        }
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
                            path.push_back(std::make_tuple(direction, tempStep));
                            break;
                        } else {
                            link.insert(newPathId);
                            std::get<2>(pathPosition[newPathId]).insert(pathId);
                            addFailReasonFunction(newX, newY, FailReason::otherPath);
                        }
                    } else {
                        nowPosition = std::make_tuple(newX, newY);
                        path.push_back(std::make_tuple(direction, tempStep));
                        mapPositionToPath[newX][newY] = pathId;
                        break;
                    }
                } else {
                    addFailReasonFunction(newX, newY, FailReason::notAllow);
                }
            }
            tempStep--;
        }
        if (pathSize == path.size()) {
            auto& [direction, distance] = path.back();
            switch (direction) {
                case Direction::up: {
                    nowPosition = std::make_tuple(nowX, nowY + distance);
                    break;
                }
                case Direction::down: {
                    nowPosition = std::make_tuple(nowX, nowY - distance);
                    break;
                }
                case Direction::left: {
                    nowPosition = std::make_tuple(nowX + distance, nowY);
                    break;
                }
                case Direction::right: {
                    nowPosition = std::make_tuple(nowX - distance, nowY);
                    break;
                }
                default: {
                    break;
                }
            }
            path.pop_back();
        }
        stepFunction();
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

const std::vector<std::tuple<FindPath::Direction, int>>& FindPath::getPath() {
    return path;
}

int FindPath::manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

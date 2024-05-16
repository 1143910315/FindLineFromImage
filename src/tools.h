#pragma once
#include <functional>
#include <vector>

namespace Tools {
    template<typename T>
    std::vector<int> findPeaks(int minPosition, int maxPosition, T threshold, std::function<T(int)> getValue) {
        std::vector<int> peaksPosition;
        bool findMinValueLocation = false;
        auto minValue = std::make_tuple(getValue(minPosition), minPosition);
        auto maxValueTuple = std::tuple(minValue);
        for (int i = minPosition + 1; i < maxPosition; ++i) {
            auto nowValue = getValue(i);
            if (!findMinValueLocation) {
                if (nowValue < std::get<0>(minValue)) {
                    minValue = std::make_tuple(nowValue, i);
                } else if (nowValue > std::get<0>(minValue) + threshold) {
                    findMinValueLocation = true;
                    maxValueTuple = minValue;
                }
            } else {
                if (nowValue > std::get<0>(maxValueTuple)) {
                    maxValueTuple = std::make_tuple(nowValue, i);
                } else if (nowValue < std::get<0>(maxValueTuple) - threshold) {
                    if (!peaksPosition.empty()) {
                        peaksPosition.push_back(std::get<1>(minValue));
                    }
                    peaksPosition.push_back(std::get<1>(maxValueTuple));
                    findMinValueLocation = false;
                    minValue = std::make_tuple(nowValue, i);
                }
            }
        }
        return peaksPosition;
    }
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    template<typename T>
    std::vector<int> findPeaks(int minPosition, int maxPosition, float threshold, std::function<T(int)> getValue) {
        std::vector<int> peaksPosition;
        bool findMinValueLocation = false;
        auto minValue = getValue(minPosition);
        std::tuple<T, int> maxValueTuple = std::make_tuple(minValue, minPosition);
        for (int i = minPosition + 1; i < maxPosition; ++i) {
            auto nowValue = getValue(i);
            if (!findMinValueLocation) {
                if (nowValue < minValue) {
                    minValue = nowValue;
                } else if (nowValue > minValue / threshold) {
                    findMinValueLocation = true;
                    maxValueTuple = std::make_tuple(nowValue, i);
                }
            } else {
                if (nowValue > std::get<0>(maxValueTuple)) {
                    maxValueTuple = std::make_tuple(nowValue, i);
                } else if (nowValue < std::get<0>(maxValueTuple) * threshold) {
                    peaksPosition.push_back(std::get<1>(maxValueTuple));
                    findMinValueLocation = false;
                    minValue = nowValue;
                }
            }
        }
        return peaksPosition;
    }
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

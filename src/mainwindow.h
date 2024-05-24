#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pathfindingvisualizer.h"
#include <QImage>
#include <QMainWindow>
#include <QThread>
#include <vector>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void debugPathFindingVisualizer(int x, int y);
public slots:
    void showStep(std::vector<std::tuple<int, int, FindPath::Direction>> path, std::vector<std::vector<std::tuple<int, int, FindPath::FailReason>>> positionFailReason);
private:
    Ui::MainWindow *ui;
    PathFindingVisualizer pathFindingVisualizer;
    QThread findPathThread;
    QImage souceImage;
    void initConnect();
    void debugImage();
    void debugFind(int x, int y);
    void debugFindPreprocessingImage(int x, int y);
    void debugSuperpositionFind(int x, int y);
};

#endif // MAINWINDOW_H

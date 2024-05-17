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
    Ui::MainWindow *ui;
    void initConnect();
    void debugImage();
    void debugFind(int x, int y);
    void debugFindPreprocessingImage(int x, int y);
};

#endif // MAINWINDOW_H

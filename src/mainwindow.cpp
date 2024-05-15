#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QImage image("image/input_image.jpg");
    // 检查图片是否成功加载
    if (!image.isNull()) {
        // 将QImage转换为QPixmap，因为QLabel的setPixmap方法需要一个QPixmap对象
        QPixmap pixmap = QPixmap::fromImage(image);
        // 设置标签的图片
        ui->label->setPixmap(pixmap);
    }
    else {
        // 如果图片加载失败，可以在这里处理错误
        ui->label->setText(tr("unload picture"));
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

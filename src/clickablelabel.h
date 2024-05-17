#pragma once
#include <QLabel>
#include <QMouseEvent>
class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit ClickableLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();
signals:
    void click(int x, int y);
protected:
    void mouseReleaseEvent(QMouseEvent *ev) override;
};

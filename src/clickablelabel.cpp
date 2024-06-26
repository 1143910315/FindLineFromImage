#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f) {
}

ClickableLabel::ClickableLabel(const QString& text, QWidget *parent, Qt::WindowFlags f) : QLabel(text, parent, f) {
}

ClickableLabel::~ClickableLabel() {
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *ev) {
    emit click(ev->x(), ev->y());
}

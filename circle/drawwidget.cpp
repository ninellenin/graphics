#include "drawwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QImage>

DrawWidget::DrawWidget(QWidget *parent, Circle* circle) :
    QWidget(parent)
{
    this->resize(100, 100);
    circle_ = circle;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

DrawWidget::~DrawWidget()
{

}

void DrawWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QImage backBuffer(width(), height(), QImage::Format_RGB888);

    uchar *pubBuffer = backBuffer.bits();
    if (!pubBuffer) {
        return;
    }
    memset(pubBuffer, 0, backBuffer.byteCount());
    //
    if (circle_) {
        circle_->Draw(&backBuffer);
    }
    //
    painter.drawImage(0, 0, backBuffer);
}

void DrawWidget::setX(int x)
{
    circle_->setX(x);
    repaint();
}

void DrawWidget::setY(int y)
{
    circle_->setY(y);
    repaint();
}

void DrawWidget::setRadius(int radius)
{
    circle_->setRadius(radius);
    repaint();
}

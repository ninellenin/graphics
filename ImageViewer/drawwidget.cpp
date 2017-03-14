#include "drawwidget.h"

#include <cmath>

#include <QPainter>
#include <QPaintEvent>
#include <QPoint>

#include <iostream>

using namespace std;

DrawWidget::DrawWidget(QWidget *parent, Image *image) :
    QWidget(parent),
    image_(image)
{
    this->resize(100, 100);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(image_, SIGNAL(updated()), this, SLOT(repaint()));
}

void DrawWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint position = event->pos();
    linearTransformation(position);

    shift_ = image_->getParameters().center() - position;
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint position = event->pos();
    linearTransformation(position);

    if (image_->getParameters().mode() == 0) {
        image_->setCenter(position + shift_);

        return;
    }

    image_->setCenter(QPoint((int)round(position.x() + shift_.x()) * image_->getParameters().scaleX(),
                      (int)round(position.y() + shift_.y()) * image_->getParameters().scaleY()));

}

void DrawWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (delta < 0) {
        image_->changeScale(0.5);
    } else {
        image_->changeScale(1.5);
    }
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

    if (image_) {
        image_->draw(backBuffer);
    }

    painter.drawImage(0, 0, backBuffer);

}

void DrawWidget::linearTransformation(QPoint &p)
{
    p.setX(p.x() - width() / 2);
    p.setY(-p.y() + height() / 2);
}

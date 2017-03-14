#include "drawwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPoint>

DrawWidget::DrawWidget(QWidget *parent, Font *font) :
    QWidget(parent),
    font_(font)
{
    this->resize(100, 100);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(font_, SIGNAL(updated()), this, SLOT(repaint()));
}

void DrawWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint position = event->pos();
    linearTransformation(position);

    shift_ = font_->getParameters().center() - position;
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event)
{
   QPoint position = event->pos();
   linearTransformation(position);

   font_->setCenter(position + shift_);

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

    if (font_) {
        font_->Draw(&backBuffer);
    }

    painter.drawImage(0, 0, backBuffer);
}

void DrawWidget::linearTransformation(QPoint &p)
{
    p.setX(p.x() - width() / 2);
    p.setY(-p.y() + height() / 2);
}

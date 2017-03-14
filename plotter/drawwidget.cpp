#include "drawwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPoint>
#include <QImage>

DrawWidget::DrawWidget(QWidget *parent, OvalOfCassini *oval) :
    QWidget(parent),
    oval_(oval),
    highlighed_(empty),
    focus_radius_(100)
{
    this->resize(100, 100);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(oval_, SIGNAL(updated()), this, SLOT(repaint()));
}

void DrawWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint position = event->pos();
    linearTransformation(position);
    long long from_f1_to_p = sqrDistance(position, oval_->getParameters().getF1());
    long long from_f2_to_p = sqrDistance(position, oval_->getParameters().getF2());

    highlighed_ = empty;

    if (from_f1_to_p <= focus_radius_ * focus_radius_) {
        if (from_f2_to_p < from_f1_to_p) {
            highlighed_ = f2;
            shift_ = oval_->getParameters().getF2() - position;
           // oval_->setF2(position);

            return;
        }

        highlighed_ = f1;
        shift_ = oval_->getParameters().getF1() - position;
       // oval_->setF1(position);

        return;
    }
    if (from_f2_to_p <= focus_radius_ * focus_radius_) {
        highlighed_ = f2;
        shift_ = oval_->getParameters().getF2() - position;
       // oval_->setF2(position);
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent *event)
{
   QPoint position = event->pos();
   linearTransformation(position);

   if (highlighed_ == f1) {
        oval_->setF1(position + shift_);
   } else if (highlighed_ == f2) {
        oval_->setF2(position + shift_);
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

    if (oval_) {
        oval_->Draw(&backBuffer);
    }

    painter.drawImage(0, 0, backBuffer);
}

long long DrawWidget::sqrDistance(const QPoint &p1, const QPoint &p2)
{
    return ((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
}


void DrawWidget::linearTransformation(QPoint &p)
{
    p.setX(p.x() - width() / 2);
    p.setY(-p.y() + height() / 2);
}

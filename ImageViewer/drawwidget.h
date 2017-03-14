#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

#include "image.h"

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    DrawWidget(QWidget *parent, Image *image);

signals:

public slots:

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *);
private:
    Image *image_;
    QPoint shift_;

    void linearTransformation(QPoint &p);
};

#endif // DRAWWIDGET_H

#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

#include "font.h"

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    DrawWidget(QWidget *parent, Font *font);

signals:

public slots:

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    Font *font_;
    QPoint shift_;

    void linearTransformation(QPoint &p);
};

#endif // DRAWWIDGET_H

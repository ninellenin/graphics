#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

#include "ovalofCassini.h"

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    DrawWidget(QWidget *parent, OvalOfCassini *oval);

signals:

public slots:

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    OvalOfCassini *oval_;
    const int focus_radius_;
    enum Highlighted {
        f1,
        f2,
        empty
    } highlighed_;
    QPoint shift_;

    void linearTransformation(QPoint &p);
    long long sqrDistance(const QPoint &p1, const QPoint &p2);
};

#endif // DRAWWIDGET_H

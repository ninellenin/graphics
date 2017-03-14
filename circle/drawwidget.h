#ifndef DRAWCIRCLE_H
#define DRAWCIRCLE_H

#include <QWidget>
#include <circle.h>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    DrawWidget(QWidget *parent, Circle*);
    ~DrawWidget();

signals:
    
public slots:
    void setX(int x);
    void setY(int x);
    void setRadius(int radius);

protected:
    void paintEvent(QPaintEvent *event);
private:
    Circle*  circle_;
};

#endif // DRAWCIRCLE_H

#ifndef CIRCLE_H
#define CIRCLE_H

#include <QObject>
#include <QImage>

class Circle : public QObject
{
    Q_OBJECT
public:
    Circle(QObject *parent = 0, int x = 100, int y = 100, int radius = 10);
    Circle(int x = 100, int y = 100, int radius = 10);
    void Draw(QImage* pBackBuffer);

signals:
    void updated();

public slots:
    void setX(int x);
    void setY(int x);
    void setRadius(int radius);
    
private:
    int x_;
    int y_;
    int radius_;

    int linear_transformation(int x, int alpha, int shift);

};

#endif // CIRCLE_H

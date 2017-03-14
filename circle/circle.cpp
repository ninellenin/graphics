#include "circle.h"

#include <math.h>

#include <iostream>

using namespace std;

using std::max;
using std::min;

Circle::Circle(QObject *parent, int x, int y, int radius) :
    QObject(parent),
    x_(x),
    y_(y),
    radius_(radius)
{

}

Circle::Circle(int x, int y, int radius) :

    x_(x),
    y_(y),
    radius_(radius)
{
    QObject(0);
}


void Circle::setX(int x)
{
    x_ = x;
    emit updated();
}

void Circle::setY(int y)
{
    y_ = y;
    emit updated();
}

void Circle::setRadius(int radius)
{
    radius_ = radius;
    emit updated();
}

int Circle::linear_transformation(int x, int alpha, int shift)
{
    return x * alpha + shift;
}

void Circle::Draw(QImage *pBackBuffer)
{
    if (!pBackBuffer)
    {
        return;
    }

    uchar* pubBuffer = pBackBuffer->bits();
    memset(pubBuffer, 255, pBackBuffer->byteCount());

    if (radius_ <= 0) {
        return;
    }

    int bufHeight = pBackBuffer->height();
    int bufWidth = pBackBuffer->width();
    if ((x_ + radius_ <= -bufWidth / 2) || (x_ - radius_ >= bufWidth / 2)) {
        return;
    }
    int sqRad = radius_ * radius_;
    int upperLine = min(y_ + radius_, bufHeight / 2);
    int bottomLine = max(y_ - radius_, -bufHeight / 2);

    for (int y = upperLine, line = bufHeight / 2 - y ; y > bottomLine; --y, ++line) {
        int sqY = (y - y_) * (y - y_);
        if (sqRad > sqY) {
            int right = (int)floor(sqrt((double)(sqRad - sqY)));
            int left = min(bufWidth / 2, max(-right + x_, - bufWidth / 2));
            right = max (- bufWidth / 2, min(right + x_, bufWidth / 2));

            memset(pubBuffer + 3 * linear_transformation(left, 1, bufWidth / 2) + (line * pBackBuffer->bytesPerLine()),
                0,
                   sizeof(uchar) * 3 * (right - left));
        }

    }
}

#include "font.h"

#include <iostream>
#include <limits>
#include <list>

using std::list;
using std::set;
using std::swap;
using std::vector;

using std::cout;
using std::endl;

Font::Font(Parameters &parameters, vector <Beziers> &curves, QObject *parent):
    QObject(parent),
    parameters_(parameters),
    curves_(curves)
{
}

Font::Font(vector <Beziers> &curves, QObject *parent):
    QObject(parent),
    curves_(curves)
{
}

void Font::Draw(QImage* pBackBuffer)
{
    if (!pBackBuffer)
    {
        return;
    }


    uchar* pubBuffer = pBackBuffer->bits();
    memset(pubBuffer, 255, pBackBuffer->byteCount());

    lines_.clear();
    for (vector<Beziers>::iterator it = curves_.begin(); it != curves_.end(); ++it) {

        int centerX = parameters_.x();
        int centerY = parameters_.y();
        Point p1((it->p1.x() + centerX) * coefficient(), (it->p1.y() + centerY) * coefficient());
        Point p2((it->p2.x() + centerX) * coefficient(), (it->p2.y() + centerY) * coefficient());
        Point p3((it->p3.x() + centerX) * coefficient(), (it->p3.y() + centerY) * coefficient());
        Beziers bez(p1, p2, p3);
        getBeziersRecursive(pBackBuffer, bez, outlineColor);

    }
    if (parameters_.filling()) {
        drawFilling(pBackBuffer, fillColor);
    }
    if (parameters_.outline()) {
        drawOutline(pBackBuffer);
    }

}

void Font::drawOutline(QImage *pBackBuffer)
{
    for (vector<Line>::iterator it = lines_.begin(); it != lines_.end(); ++it)
    {
        //setPoint(pBackBuffer, it->first, outlineColor);
        //setPoint(pBackBuffer, it->second, outlineColor);
        drawLine(pBackBuffer, it->first, it->second, outlineColor);
    }

}

void Font::drawPoint(QImage *pBackBuffer, const QPoint &p, int diameter)
{
    uchar blueColor[3] = {0, 0, 255};
    uchar* pubBuffer = pBackBuffer->bits();
    int centralY = pBackBuffer->height() / 2;
    int centralX = pBackBuffer->width() / 2;

    if ((abs(p.x()) > centralX) || (abs(p.y()) > centralY)) {
        return;
    }

    for (int i = -diameter / 2; i < (diameter - 1) / 2; ++i) {
        for (int j = -diameter / 2; j < (diameter - 1) / 2; ++j) {
            memcpy(pubBuffer + (linearTransformation(p.y() + i, -1, centralY) * pBackBuffer->bytesPerLine()) +
                   (linearTransformation(p.x() + j, 1, centralX) * 3),
                blueColor, 3);
        }
    }
}

bool Font::isCollinear(const Beziers &beziers)
{
    return !((beziers.p2.x() - beziers.p1.x()) * (beziers.p3.y() - beziers.p2.x()) -
            (beziers.p2.y() - beziers.p1.y()) * (beziers.p3.x() - beziers.p2.x()));
}

inline void Font::drawLine(QImage *pBackBuffer, const QPoint &p1, const QPoint &p2, const uchar color[])
{
    QPoint current(p1);
    QPoint direction(p2 - p1);

    QPoint next[3];
    QPoint delta[3];

    delta[0].setX((direction.x() > 0)? 1: -1);
    delta[0].setY(0);
    delta[1].setX(0);
    delta[1].setY((direction.y() > 0)? 1: -1);
    delta[2].setX((direction.x() > 0)? 1: -1);
    delta[2].setY((direction.y() > 0)? 1: -1);

    setPoint(pBackBuffer, current, color);
    while (current != p2){
        next[0] = next[1] = next[2] = current;
        int min_err = std::numeric_limits<int>::max();
        int err;
        for (int i = 0; i < 3; ++i) {
            next[i]+= delta[i];
            err = abs(direction.x()*(p1.y() - next[i].y()) - direction.y()*(p1.x() - next[i].x()));
            if (err < min_err) {
                min_err = err;
                current = next[i];
            }
        }
        setPoint(pBackBuffer, current, color);
    }
    setPoint(pBackBuffer, current, color);
}

void Font::drawFilling(QImage *pBackBuffer, const uchar color[])
{
    int centralY = pBackBuffer->height() / 2;
    for (int y = - centralY; y != centralY; ++y) {
        set<int> points;
        getIntersections(y, points);

        for (set<int>::iterator it = points.begin(); it != points.end(); ++it) {
            int start = *it;
            if (++it == points.end()) {
                break;
            }
            setHorizontalLine(pBackBuffer, y, start, *it, color);
        }

    }

}


void Font::getIntersections(int y, set<int> &result)
{
    for (vector<Line>::iterator it = lines_.begin(); it != lines_.end(); ++it) {
        QPoint first = it->first;
        QPoint second = it->second;
        if (first.y() > second.y()) {
            swap(first, second);
        } else if (first.y() == second.y()) {
            continue;
        }

        if ((first.y() <= y) && (second.y() > y)){
            int x = (int)round((y * (second.x() - first.x()) + first.x() * second.y() - second.x() * first.y())
                                / (double)(second.y() - first.y()));
            result.insert(x);
        }
    }
}

void Font::getBeziersRecursive(QImage *pBackBuffer, const Beziers &beziers,  uchar color[], int level)
{
    if (isCollinear(beziers)) {
         lines_.push_back(Line(beziers.p1.getQPoint(), beziers.p3.getQPoint()));

         return;
    }
    Point p1(beziers.p1);
    Point p2(beziers.p2);
    Point p3(beziers.p3);

    double d = ((p3.x() - p2.x()) * (p1.y() - p3.y()) + (p3.y() - p2.y()) * (p3.x() - p1.x()));

    if (d * d <= 0.25 * (distance(p1, p3))) {
        if (p1.getQPoint() != p3.getQPoint()) {
            lines_.push_back(Line(p1.getQPoint(), p3.getQPoint()));
        }

        return;
     }

    Beziers b1(p1, beziers.p12, beziers.p123);
    Beziers b2(beziers.p123, beziers.p23, p3);

    getBeziersRecursive(pBackBuffer, b1, color, level + 1);
    getBeziersRecursive(pBackBuffer, b2, color, level + 1);
}

vector<Font::Beziers> &Font::getCurves()
{
    return curves_;
}

const Font::Parameters &Font::getParameters() const
{
    return parameters_;
}

inline int Font::distance(const QPoint &p1, const QPoint &p2)
{
    return ((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
}


void Font::setPoint(QImage *pBackBuffer, const QPoint &point, const uchar color[])
{
    int centralX = pBackBuffer->width() / 2;
    int centralY = pBackBuffer->height() / 2;

    if ((centralX < std::abs(point.x())) || (centralY < abs(point.y()))) {
        return;
    }
    memcpy(pBackBuffer->bits() +
           (linearTransformation(point.y(), -1, centralY) * pBackBuffer->bytesPerLine()) +
           (linearTransformation(point.x(), 1, centralX)) * 3, color, 3);
}

inline void Font::setHorizontalLine(QImage *pBackBuffer, int y, int start, int end, const uchar color[])
{
    int centralX = pBackBuffer->width() / 2;
    int centralY = pBackBuffer->height() / 2;

    if (centralY <= std::abs(y)) {
        return;
    }
    if (centralX < std::abs(start)) {
        start = (start > 0)? centralX: -centralX ;
    }
    if (centralX < std::abs(end)) {
        end = (end > 0)? centralX: -centralX;
    }

    if (start > end) {
        swap(start, end);
    }

    for (; start != end; ++start) {
        memcpy(pBackBuffer->bits() +
               (linearTransformation(y, -1, centralY) * pBackBuffer->bytesPerLine()) +
               (linearTransformation(start, 1, centralX)) * 3, color, 3);
    }

}

double Font::coefficient()
{
    double coefficient;
    int scale = parameters_.scale();

    if (scale <= 0) {
        coefficient = 1.0 + scale / 1000.0;
    } else {
        coefficient = 1.0 + scale / 100.0;
    }
    return coefficient;
}

int Font::linearTransformation(int x, int alpha, int shift)
{
    return x * alpha + shift;
}

void Font::setX(int x)
{
    parameters_.setX(x);

    emit updated();
}

void Font::setY(int y)
{
    parameters_.setY(y);

    emit updated();
}
void Font::setCenter(const QPoint &other)
{
    parameters_.setCenter(other);

    emit updated();

}

void Font::setScale(int scale)
{
    parameters_.setScale(scale);

    emit updated();
}

void Font::setFilling(int state)
{
    parameters_.setFilling(state);

    emit updated();
}

void Font::setOutline(int state)
{
    parameters_.setOutline(state);

    emit updated();
}

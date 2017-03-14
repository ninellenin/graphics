#include "ovalofCassini.h"

#include <limits>
#include <numeric>

using std::abs;
using std::vector;

OvalOfCassini::OvalOfCassini(Parameters &parameters, QObject *parent) :
    QObject(parent),
    parameters_(parameters)
{
}

OvalOfCassini::OvalOfCassini(QObject *parent) :
     QObject(parent)
{
}

int OvalOfCassini::linearTransformation(int x, int alpha, int shift)
{
    return x * alpha + shift;
}

void OvalOfCassini::Draw(QImage* pBackBuffer)
{
    if (!pBackBuffer)
    {
        return;
    }

    uchar* pubBuffer = pBackBuffer->bits();
    memset(pubBuffer, 255, pBackBuffer->byteCount());

    drawAxes(pBackBuffer);
    drawPoint(pBackBuffer, parameters_.getF1(), 4);
    drawPoint(pBackBuffer, parameters_.getF2(), 4);

    QPoint f2_to_f1(parameters_.getF1().x() - parameters_.getF2().x(),
                   parameters_.getF1().y() - parameters_.getF2().y());
    QPoint p1 = findPoint(pBackBuffer, parameters_.getF1(), f2_to_f1);
    QPoint orthogonal1(f2_to_f1.y(), -f2_to_f1.x());
    QPoint orthogonal2(-f2_to_f1.y(), f2_to_f1.x());
    drawPlotInQuater(pBackBuffer, p1, orthogonal1, f2_to_f1);
    drawPlotInQuater(pBackBuffer, p1, orthogonal2, f2_to_f1);

    QPoint f1_to_f2(parameters_.getF2().x() - parameters_.getF1().x(),
                   parameters_.getF2().y() - parameters_.getF1().y());
    QPoint p2 = findPoint(pBackBuffer, parameters_.getF2(), f1_to_f2);
    drawPlotInQuater(pBackBuffer, p2, orthogonal1, f1_to_f2);
    drawPlotInQuater(pBackBuffer, p2, orthogonal2, f1_to_f2);
}

void OvalOfCassini::drawPoint(QImage *pBackBuffer, const QPoint &p, int diameter)
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

QPoint OvalOfCassini::getVector(const QPoint &from, const QPoint &to) const
{
    return QPoint(to.x() - from.x(), to.y() - from.y());
}

void OvalOfCassini::drawPlotInQuater(QImage *pBackBuffer, const QPoint &start, const QPoint &direction, const QPoint &quater)
{
    int x1 = parameters_.getF1().x();
    int y1 = parameters_.getF1().y();
    int x2 = parameters_.getF2().x();
    int y2 = parameters_.getF2().y();

    QPoint current_direction;

    current_direction.setX(sign(direction.x()));
    current_direction.setY(sign(direction.y()));

    uchar *pubBuffer = pBackBuffer->bits();
    uchar color[3] = {255, 0, 255};
    QPoint p = start;

    int centralY = pBackBuffer->height() / 2;
    int centralX = pBackBuffer->width() / 2;
    int bytesPerLine = pBackBuffer->bytesPerLine();
    vector <QPoint> points;
    vector <long long> errs;
    int count = 0;
    QPoint previous = p;
    QPoint radius_vector;
    do {
        int current_x = p.x();
        int current_y = p.y();
        if ((abs(current_y) <= centralY) && (abs(current_x) <= centralX)) {
            memcpy(pubBuffer + bytesPerLine * linearTransformation(current_y, -1, centralY) +
                linearTransformation(current_x, 1, centralX) * 3, color, 3);
        }

        for (int k = 0, i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if ((i == 0) && (j == 0)) {
                    continue;
                }

                if (sqr(i - current_direction.x()) + sqr(j - current_direction.y()) <= 2) {
                    points.push_back(p);
                    points[k].setX(current_x + i);
                    points[k].setY(current_y + j);
                    errs.push_back(abs(error(points[k])));
                    ++k;
                }
            }
        }
        int minimum = std::min_element(errs.begin(), errs.end()) - errs.begin();

        previous = p;
        QPoint point = (points[minimum] - p);
        current_direction = point;
        p = points[minimum];
        points.clear();
        errs.clear();

        radius_vector.setX(2 * p.x() - x1 - x2);
        radius_vector.setY(2 * p.y() - y1 - y2);

    } while ((innerProduct(radius_vector, direction) >= 0) &&
             (innerProduct(radius_vector, quater) >= 0) &&
             (++count < 1000));
}

void OvalOfCassini::drawAxes(QImage* pBackBuffer)
{
    uchar* pubBuffer = pBackBuffer->bits();
    int centralY = pBackBuffer->height() / 2;
    int centralX = pBackBuffer->width() / 2;
    uchar redColor[3] = {255, 0, 0};
    uchar blueColor[3] = {0, 0, 255};
    int bytesPerLine = pBackBuffer->bytesPerLine();
    int height = pBackBuffer->height();
    int width = pBackBuffer->width();
    for (int i = 0; i < height; ++i) {
        if (i != centralY) {
            memcpy(pubBuffer + bytesPerLine * i + centralX * 3, redColor, 3);
        } else {
            for (int j = 0; j < width; ++j) {
                memcpy(pubBuffer + bytesPerLine * i + j * 3, redColor, 3);
            }
        }
    }
}

int OvalOfCassini::sign(int number)
{
    if (number > 0) {
        return 1;
    }

    if (number == 0) {
        return 0;
    }

    return -1;
}

QPoint OvalOfCassini::findPoint(QImage *pBackBuffer, const QPoint &p, const QPoint &direction)
{
    QPoint result(p);
    QPoint previous;
    QPoint next[3];
    QPoint delta[3];

    delta[0].setX((direction.x() > 0)? 1: -1);
    delta[0].setY(0);
    delta[1].setX(0);
    delta[1].setY((direction.y() > 0)? 1: -1);
    delta[2].setX((direction.x() > 0)? 1: -1);
    delta[2].setY((direction.y() > 0)? 1: -1);

    long long current_error;
    long long next_error = 0;

    for (;; previous = result) {
        current_error = next_error;

        next[0] = next[1] = next[2] = result;
        int min_err = std::numeric_limits<int>::max();
        int err;
        for (int i = 0; i < 3; ++i) {
            next[i]+= delta[i];
            err = std::abs(direction.x()*(p.y() - next[i].y()) - direction.y()*(p.x() - next[i].x()));
            if (err < min_err) {
                min_err = err;
                result = next[i];
            }
        }
        next_error = error(result);
        if ((next_error * current_error <= 0) &&    // different signs of errors or error == 0
                (current_error > 0)) {              // but step isn't first
            if (abs(next_error) > abs(current_error)) {
                return previous;
            }

            return result;
        }
    }

}

inline long long OvalOfCassini::sqr(int x)
{
    return x * x;
}

long long OvalOfCassini::innerProduct(const QPoint &mult1, const QPoint &mult2)
{
    return (mult1.x() * mult2.x() + mult1.y() * mult2.y());
}

long long OvalOfCassini::error(const QPoint &p)
{
    long long sqrR1 = (sqr(p.x() - parameters_.getX1()) + sqr(p.y() - parameters_.getY1()));
    long long sqrR2 = (sqr(p.x() - parameters_.getX2()) + sqr(p.y() - parameters_.getY2()));

    return (sqr(parameters_.getR()) - sqrR1 * sqrR2);
}

void OvalOfCassini::setF1(const QPoint &f1)
{
    setX1(f1.x());
    setY1(f1.y());
}

void OvalOfCassini::setF2(const QPoint &f2)
{
    setX2(f2.x());
    setY2(f2.y());
}


void OvalOfCassini::setX1(int x1)
{
    parameters_.setX1(x1);

    emit updated();
}

void OvalOfCassini::setX2(int x2)
{
    parameters_.setX2(x2);

    emit updated();
}

void OvalOfCassini::setY1(int y1)
{
    parameters_.setY1(y1);

    emit updated();
}

void OvalOfCassini::setY2(int y2)
{
    parameters_.setY2(y2);

    emit updated();
}

void OvalOfCassini::setR(int r)
{
    parameters_.setR(r);

    emit updated();
}


const OvalOfCassini::Parameters &OvalOfCassini::getParameters() const
{
    return parameters_;
}

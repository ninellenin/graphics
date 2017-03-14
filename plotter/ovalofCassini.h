#ifndef OVALOFCASSINI_H
#define OVALOFCASSINI_H

#include <vector>

#include <QImage>
#include <QObject>
#include <QPoint>

class OvalOfCassini : public QObject
{
    Q_OBJECT
public:

    class Parameters {
    private:
        QPoint f1_;
        QPoint f2_;
        int r_;
    public:
        Parameters(int x1 = -10, int y1 = 0, int x2 = 10, int y2 = 0, int r = 100) :
            f1_(x1, y1),
            f2_(x2, y2),
            r_(r)
        {}

        Parameters(const QPoint &f1, const QPoint &f2, int r = 10) :
            f1_(f1),
            f2_(f2),
            r_(r)
        {}

        Parameters(const Parameters &other) :
            f1_(other.getX1(), other.getY1()),
            f2_(other.getX2(), other.getY2()),
            r_(other.getR())
        {

        }

        const QPoint &getF1() const
        {
            return f1_;
        }
        const QPoint &getF2() const
        {
            return f2_;
        }
        int getX1() const
        {
            return f1_.x();
        }
        int getX2() const
        {
            return f2_.x();
        }
        int getY1() const
        {
            return f1_.y();
        }
        int getY2() const
        {
            return f2_.y();
        }
        int getR() const
        {
            return r_;
        }

        void setX1(int x1)
        {
            f1_.setX(x1);
        }
        void setX2(int x2)
        {
            f2_.setX(x2);
        }
        void setY1(int y1)
        {
            f1_.setY(y1);
        }
        void setY2(int y2)
        {
            f2_.setY(y2);
        }
        void setR(int r)
        {
            r_ = r;
        }
    };

    OvalOfCassini(Parameters &parameters, QObject *parent = 0);
    OvalOfCassini(QObject *parent = 0);
    void Draw(QImage* pBackBuffer);
    const Parameters &getParameters() const;
    void setF1(const QPoint &f1);
    void setF2(const QPoint &f2);

signals:
    void updated();
public slots:
    void setX1(int x1);
    void setX2(int x2);
    void setY1(int y1);
    void setY2(int y2);
    void setR(int r);

private:
    Parameters parameters_;

    int linearTransformation(int x, int alpha, int shift);
    void drawAxes(QImage* pBackBuffer);
    QPoint findPoint(QImage *pBackBuffer, const QPoint &p, const QPoint &direction);
    inline long long sqr(int x);
    long long error(const QPoint &p);
    void drawPoint(QImage *pBackBuffer, const QPoint &p, int diameter = 6);
    void drawPlot(QImage *pBackBuffer, const QPoint &start, QPoint direction);
    void drawPlotInQuater(QImage *pBackBuffer, const QPoint &start, const QPoint &direction, const QPoint &quater);
    QPoint getVector(const QPoint &from, const QPoint &to) const;
    bool isOutside(const QPoint &point, int x, int y);
    inline int sign(int number);
    long long innerProduct(const QPoint &mult1, const QPoint &mult2);
};

#endif // OVALOFCASSINI_H

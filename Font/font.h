#ifndef FONT_H
#define FONT_H

#include <cmath>
#include <set>
#include <vector>

#include <QImage>
#include <QObject>
#include <QPoint>

class Font : public QObject
{
    Q_OBJECT
public:
    class Point {
    private:
        double x_;
        double y_;
    public:
        Point()
        {
        }
        Point(int x, int y) :
            x_(x),
            y_(y)
        {
        }
        explicit Point(const QPoint &point) :
            x_(point.x()),
            y_(point.y())
        {
        }
        Point(double x, double y) :
            x_(x),
            y_(y)
        {
        }
        double x() const
        {
            return x_;
        }
        double y() const
        {
            return y_;
        }
        void setX(int x)
        {
            x_ = x;
        }
        void setY(int y)
        {
            y_ = y;
        }
        void setX(double x)
        {
            x_ = x;
        }
        void setY(double y)
        {
            y_ = y;
        }
        QPoint getQPoint() const
        {
            QPoint point((int)round(x_), (int)round(y_));

            return point;
        }

        Point & operator=(const Point &other)
        {
            x_ = other.x();
            y_ = other.y();

            return *this;
        }

        Point & operator=(const QPoint &other)
        {
            x_ = other.x();
            y_ = other.y();

            return *this;
        }

        bool operator==(const Point &other) const
        {
            return ((x() == other.x()) && (y() == other.y()));
        }

        bool operator!=(const Point &other) const
        {
            return (!(*this == other));
        }

        bool operator!=(const QPoint &other) const
        {
            return (Point(other) != *this);
        }

        Point &operator+=(const Point &other)
        {
            setX(x() + other.x());
            setY(y() + other.y());

            return *this;
        }
        Point operator+(const Point &other) const
        {
            Point sum(x_ + other.x(), y_ + other.y());

            return sum;
        }
        Point & operator/(double divisor)
        {
            x_ /= divisor;
            y_ /= divisor;

            return *this;
        }
        bool operator <(const Point &other) const
        {
            if (y() < other.y()) {
                return true;
            } else if (y() > other.y()) {
                return false;
            }
            if (x() < other.x()) {
                return true;
            }

            return false;
        }
    };
    class Parameters {
    private:
        QPoint center_;
        int scale_;
        bool filling_;
        bool outline_;
    public:
        Parameters(int x = 0, int y = 0, int scale = -50, bool filling = true, bool outline = true) :
            center_(x, y),
            scale_(scale),
            filling_(filling),
            outline_(outline)
        {
        }

        Parameters(const QPoint &center, int scale = 1, bool filling = true, bool outline = true) :
            center_(center),
            scale_(scale),
            filling_(filling),
            outline_(outline)
        {}

        Parameters(const Parameters &other) :
            center_(other.x(), other.y()),
            scale_(other.scale()),
            filling_(other.filling()),
            outline_(other.outline())
        {

        }

        const QPoint &center() const
        {
            return center_;
        }

        int x() const
        {
            return center_.x();
        }

        int y() const
        {
            return center_.y();
        }

        int scale() const
        {
            return scale_;
        }

        bool filling() const
        {
            return filling_;
        }

        bool outline() const
        {
            return outline_;
        }

        void setX(int x)
        {
            center_.setX(x);
        }

        void setY(int y) {
            center_.setY(y);
        }

        void setScale(int scale) {
            scale_ = scale;
        }

        void setFilling(bool filling) {
            filling_ = filling;
        }

        void setOutline(bool outline) {
            outline_ = outline;
        }
        void setCenter(const QPoint &other)
        {
            center_ = other;
        }
    };
    struct Beziers {
        Point p1;
        Point p2;
        Point p3;
        Point p12;
        Point p23;
        Point p123;
    public:
        Beziers(const Point &p1, const Point &p2, const Point &p3) :
            p1(p1),
            p2(p2),
            p3(p3)
        {
            p12 = (p1 + p2) / 2.0;
            p23 = (p2 + p3) / 2.0;
            p123 = (p12 + p23) / 2.0;
        }

        Beziers & operator =(const Beziers &other)
        {
            p1 = other.p1;
            p2 = other.p2;
            p3 = other.p3;

            return *this;
        }
    };

    Font(Parameters &parameters, std::vector <Beziers> &curves, QObject *parent = 0);
    Font(std::vector <Beziers> &curves, QObject *parent = 0);

    void Draw(QImage* pBackBuffer);
    std::vector <Beziers> &getCurves();
    void drawPoint(QImage *pBackBuffer, const QPoint &p, int diameter = 6);
    void getBeziersRecursive(QImage *pBackBuffer, const Beziers &beziers, uchar color[], int level = 0);
    static double distance(const Point &p1, const Point &p2)
    {
        return ((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()));
    }
    const Parameters &getParameters() const;

    Font &operator=(const Font &other)
    {
        parameters_ = other.getParameters();
        curves_ = getCurves();
    }

signals:
    void updated();

public slots:
    void setX(int x);
    void setY(int y);
    void setCenter(const QPoint &other);
    void setScale(int scale);
    void setFilling(int state);
    void setOutline(int state);
protected:
    void addLine(const QPoint &p1, const QPoint &p2);
    inline void setPoint(QImage *pBackBuffer, const QPoint &point, const uchar color[]);
    inline void drawLine(QImage *pBackBuffer, const QPoint &p1, const QPoint &p2, const uchar color[]);
    inline void setHorizontalLine(QImage *pBackBuffer, int y, int start, int end, const uchar color[]);
    void drawFilling(QImage *pBackBuffer, const uchar color[]);
    void drawOutline(QImage *pBackBuffer);
    void getIntersections(int y, std::set<int> &result);

    const double kEpsilon = 1e-30;
    const double kDistanceTolerance = 0.25;
    const double kPi = 3.141592653589793238462643383279502884197169399;
    const double kAngleTolerance = kPi / 360;
    uchar outlineColor[3] = {0, 0, 0};
    uchar fillColor[3] = {255, 0, 0};

   private:
    bool isCollinear(const Beziers &beziers);
    inline int linearTransformation(int x, int alpha, int shift);
    inline int distance(const QPoint &p1, const QPoint &p2);
    inline double coefficient();

    typedef std::pair<QPoint, QPoint> Line;

    Parameters parameters_;
    std::vector <Beziers> curves_;
    std::vector<Line> lines_;
};

#endif // FONT_H

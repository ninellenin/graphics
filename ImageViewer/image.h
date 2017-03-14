#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QObject>
#include <QPoint>

class Image : public QObject
{
    Q_OBJECT
public:
    class Parameters {
    public:
        enum Mode {
            SIMPLE = 0,
            BILINEAR = 1,
            MIPMAPPING = 2
        };
    private:
        QPoint center_;
        double scaleX_;
        double scaleY_;
        Mode mode_;
    public:

        Parameters(int x = 0, int y = 0, double scaleX = 1, double scaleY = 1, Mode mode = SIMPLE) :
            center_(x, y),
            scaleX_(scaleX),
            scaleY_(scaleY),
            mode_(mode)
        {
        }

        Parameters(const QPoint &center, double scaleX = 1, double scaleY = 1, Mode mode = SIMPLE) :
            center_(center),
            scaleX_(scaleX),
            scaleY_(scaleY),
            mode_(mode)
        {}

        Parameters(const Parameters &other) :
            center_(other.x(), other.y()),
            scaleX_(other.scaleX()),
            scaleY_(other.scaleY()),
            mode_(other.mode())
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

        double scaleX() const
        {
            return scaleX_;
        }

        double scaleY() const
        {
            return scaleY_;
        }

        Mode mode() const
        {
            return mode_;
        }

        void setX(int x)
        {
            center_.setX(x);
        }

        void setY(int y) {
            center_.setY(y);
        }

        void setScaleX(double scale) {
            if (scale > 10.0) {
                scaleX_ = 10.0;

                return;
            } else if (scale < 0){
                scaleX_ = 0;

                return;
            }
            scaleX_ = scale;
        }

        void setScaleY(double scale) {
            if (scale > 10.0) {
                scaleY_ = 10.0;

                return;
            } else if (scale < 0){
                scaleY_ = 0;

                return;
            }
            scaleY_ = scale;
        }

        void setCenter(const QPoint &other)
        {
            center_ = other;
        }

        void setMode(Mode mode)
        {
            mode_ = mode;
        }
    };
    struct ImageBuffer {
        int *values;
        int width;
        int height;
    public:
        ImageBuffer(int *values, int width, int height):
            values(values),
            width(width),
            height(height)
        {
        }

        const int operator()(int x, int y) const
        {
            return values[width * y + x];
        }

        int &operator()(int x, int y)
        {
            return values[width * y + x];
        }
    };

    Image(Parameters &parameters, const QImage &image, QObject *parent = 0);
    Image(const QImage &image, QObject *parent = 0);

    const Parameters &getParameters() const;
    void draw(QImage &backBuffer);
    void biLinear(const QImage &image, QImage &backBuffer, double scaleX, double scaleY);
    void biLinearTransformation(const ImageBuffer &in, ImageBuffer &out, double scaleX, double scaleY);
    void showImage(QImage &screen, const QImage &image, const QPoint &shift);
    void generateMipmap(QImage image, QImage &result);
    void getFromMipmap(const QImage &mipmap, QImage &image, int level);
    void setCenter(const QPoint &center);
    void changeScale(double mult);
    QImage getTrimmedImage(const QImage &image, const QImage &screen, const QPoint &center, double scaleX, double scaleY);
    void biLin(const QImage &image, QImage &backBuffer);
    void biLinearTransform(const ImageBuffer &in, ImageBuffer &out);
    int biLinearInterpolation(double x, double y, int x00, int x01, int x10, int x11);
    int biCubicTransformation(double x, double y, int *values);

signals:
    void updated();

public slots:
    void setX(int x);
    void setY(int y);
    void setScaleX(double scale);
    void setScaleY(double scale);
    void setMode(int mode);
private:
    Parameters parameters_;
    QImage image_;
    std::vector<QImage> mipmaps_;

    inline int linearTransformation(int x, int alpha, int shift);
    inline int calculateSize(int size, double scale);
    void splitColors(const QImage &image, ImageBuffer &red, ImageBuffer &green, ImageBuffer &blue);
    void joinColors(QImage &image, const ImageBuffer &red, const ImageBuffer &green, const ImageBuffer blue);
    void reduceInHalf(const QImage &image, QImage &result);
    int getLevel(int realSize, int size);
    void initMipmaps();
    QRgb getColor(const QImage &image, double u, double v);
};

#endif // IMAGE_H

#include "image.h"

#include <cmath>
#include <limits>
#include <QDebug>

using std::max;
using std::min;
using std::numeric_limits;

#include <iostream>

using namespace std;

Image::Image(Parameters &parameters, const QImage &image, QObject *parent):
    Image(image, parent)
{

}

Image::Image(const QImage &image, QObject *parent):
    QObject(parent),
    image_(image)
{
    image_.convertToFormat(QImage::Format_RGB888);
    if (image_.width() != image.height()) {
        int min = std::min(image.width(), image.height());
        min = std::min(min, min - (min % 2));
        image_ = image_.copy(0, 0, min, min);
    }
    initMipmaps();
}

void Image::initMipmaps()
{
    int size = image_.height();
    QImage mipmap(size * 2, size, QImage::Format_RGB888);
    generateMipmap(image_, mipmap);

    mipmaps_.push_back(image_);
    for (int level = 0; size; size /= 2, ++level) {
        QImage image(size / 2, size / 2, QImage::Format_RGB888);
        getFromMipmap(mipmap, image, level);
        mipmaps_.push_back(image);
     }

}

void Image::draw(QImage &backBuffer)
{
    int size = image_.width();
    int width = image_.width() * parameters_.scaleX();
    int height = image_.height() * parameters_.scaleY();

    int centralX1 = backBuffer.width() / 2;
    int centralY1 = backBuffer.height() / 2;
    int centralX2 = image_.width() / 2;
    int centralY2 = image_.height() / 2;

    for (int y = - centralY1 + 1; y < centralY1; ++y) {
        for (int x = - centralX1; x < centralX1; ++x) {
            backBuffer.setPixel(linearTransformation(x, 1, centralX1),
                                linearTransformation(y, -1, centralY1),
                                getColor(image_,
                                         linearTransformation(x - parameters_.x() / parameters_.scaleX(), 1, centralX2) / (double)width,
                                         linearTransformation(y - parameters_.y() / parameters_.scaleY(), -1, centralY2) / (double)height));
        }
    }



      //  QImage image(scaledImage.width(), scaledImage.height(), QImage::Format_RGB888);
        //image.fill(qRgb(0, 0, 0));

        //biLin(backBuffer, backBuffer);
        //showImage(backBuffer, image, QPoint((int)round(parameters_.x() / parameters_.scaleX()), (int)round(parameters_.y() / parameters_.scaleY())));//parameters_.center());

/*
        QImage trimmedImage = getTrimmedImage(image_, backBuffer, parameters_.center(), parameters_.scaleX(), parameters_.scaleY());
        int sizeX = calculateSize(trimmedImage.width(), parameters_.scaleX()) - 3 * parameters_.scaleX();
        int sizeY = calculateSize(trimmedImage.height(), parameters_.scaleY()) - 3 * parameters_.scaleY();
        QImage image(sizeX, sizeY, QImage::Format_RGB888);
        image.fill(qRgb(0, 0, 0));

        biLinear(trimmedImage, image, parameters_.scaleX(), parameters_.scaleY());
        showImage(backBuffer, image, QPoint((int)round(parameters_.x() / parameters_.scaleX()), (int)round(parameters_.y() / parameters_.scaleY())));//parameters_.center());
*/
    /*} else {
        int level = getLevel(size, (int)round(size * min(parameters_.scaleX(), parameters_.scaleY())));
        cout << "level = " << level << endl;

        QImage trimmedImage = getTrimmedImage(mipmaps_[level], backBuffer, parameters_.center(), parameters_.scaleX(), parameters_.scaleY());

        int sizeX = calculateSize(trimmedImage.width(), parameters_.scaleX()) - 3 * parameters_.scaleX();
        int sizeY = calculateSize(trimmedImage.height(), parameters_.scaleY()) - 3 * parameters_.scaleY();
        QImage image(sizeX, sizeY, QImage::Format_RGB888);
        image.fill(qRgb(0, 0, 0));

        biLinear(trimmedImage, image, parameters_.scaleX(), parameters_.scaleY());
        showImage(backBuffer, image, QPoint((int)round(parameters_.x() / parameters_.scaleX()), (int)round(parameters_.y() / parameters_.scaleY())));
    }*/

    return;

}

QRgb Image::getColor(const QImage &image, double u, double v)
{
    if ((u >= 1) || (u < 0) || (v >= 1) || (v < 0)) {
        return qRgb(0, 0, 0);
    }
    int x = (int)(u * image.width());
    int y = (int)(v * image.height());
    if (parameters_.mode() == 0) {
        return image.pixel(x, y);
    } else if (parameters_.mode() == 1) {
        if ((x + 1 == image.width()) || (y + 1 == image.height())) {
            return qRgb(0, 0, 0);
        }

        int red = biLinearInterpolation(u * image.width() - x, v * image.height() - y, qRed(image.pixel(x, y)), qRed(image.pixel(x , y + 1)),
                                        qRed(image.pixel(x + 1, y)), qRed(image.pixel(x + 1, y + 1)));

        int green = biLinearInterpolation(u * image.width() - x, v * image.height() - y, qGreen(image.pixel(x, y)), qGreen(image.pixel(x, y + 1)),
                                  qGreen(image.pixel(x + 1, y)), qGreen(image.pixel(x + 1, y + 1)));

        int blue = biLinearInterpolation(u * image.width() - x, v * image.height() - y, qBlue(image.pixel(x, y)), qBlue(image.pixel(x, y + 1)),
                                    qBlue(image.pixel(x + 1, y)), qBlue(image.pixel(x + 1, y + 1)));

        return (qRgb(red, green, blue));
    }

}

int Image::biLinearInterpolation(double x, double y, int x00, int x01, int x10, int x11)
{

    double result = (x00 * (1. - x) * (1. - y) + x01 * (1. - x) * y + x10 * x * (1. - y) + x11 * x * y);

    int res;
    if (result > 255.) {
        res = 255.;
    } else if (result < 0.) {
        res = 0;
    } else {
        res = (int)round(result);
    }

    return res;
}

int Image::biCubicTransformation(double x, double y, int *values)
{
    double c[4][4];
    ImageBuffer in(values, 4, 4);
    c[0][0] = in(x + 1, y + 1);
    c[0][1] = (- 0.5) * in(x + 1, y)
            + 0.5 * in(x + 1, y + 2);
        c[0][2] = in(x + 1, y)
            + (- 2.5) * in(x + 1, y + 1)
            + 2 * in(x + 1, y + 2)
            + (- 0.5) * in(x + 1, y + 3);
        c[0][3] = (-0.5) * in(x + 1, y)
            + 1.5 * in(x + 1, y + 1)
            + (- 1.5) * in(x + 1, y + 2)
            + 0.5 * in(x + 1, y + 3);
    c[1][0] = (-0.5) * in(x, y + 1)
        + 0.5 * in(x + 2, y + 1);
    c[1][1] = 0.25 * in(x, y)
        + (- 0.25) * in(x, y + 2)
        + (- 0.25) * in(x + 2, y)
        + 0.25 * in(x + 2, y + 2);
    c[1][2] = (- 0.5) * in(x, y)
        + 1.25 * in(x, y + 1)
        - in(x, y + 2)
        + 0.25 * in(x, y + 3)
        + 0.5 * in(x + 2, y)
        + (- 1.25) * in(x + 2, y + 1)
        + in(x + 2, y + 2)
        + (- 0.25) * in(x + 2, y + 3);
    c[1][2] = (- 0.5) * in(x, y)
        + 1.25 * in(x, y + 1)
        - in(x, y + 2)
        + 0.25 * in(x, y + 3)
        + 0.5 * in(x + 2, y)
        + (- 1.25) * in(x + 2, y + 1)
        + in(x + 2, y + 2)
        + (- 0.25) * in(x + 2, y + 3);
    c[1][3] = 0.25 * in(x, y)
        + (- 0.75) * in(x, y + 1)
        +  0.75 * in(x, y + 2)
        + (-0.25) * in(x, y + 3)
        + (-0.25) * in(x + 2, y)
        + 0.75 * in(x + 2, y + 1)
        + (- 0.75) * in(x + 2, y + 2)
        + 0.25 * in(x + 2, y + 3);
    c[2][0] = in(x, y + 1)
        + (- 2.5) * in(x + 1, y + 1)
        +  2 * in(x + 2, y + 1)
        + (- 0.5) * in(x + 3, y + 1);
    c[2][1] = (- 0.5) * in(x, y)
        + 0.5 * in(x, y + 2)
        +  1.25 * in(x + 1, y)
        + (-1.25) * in(x + 1, y + 2)
        - in(x + 2, y)
        + in(x + 2, y + 2)
        + 0.25 * in(x + 3, y)
        + (- 0.25) * in(x + 3, y + 2);
    c[2][2] = in(x, y)
        + (- 2.5) * in(x, y + 1)
        +  2 * in(x, y + 2)
        + (-0.5) * in(x, y + 3)
        + (- 2.5) * in(x + 1, y)
        +  6.25 * in(x + 1, y + 1)
        +  (- 5) * in(x + 1, y + 2)
        + 1.25 * in(x + 1, y + 3)
        +  2 * in(x + 2, y)
        +  (-5) * in(x + 2, y + 1)
        + 4 * in(x + 2, y + 2)
        - in(x + 2, y + 3)
        + (- 0.5) * in(x + 3, y)
        +  1.25 * in(x + 3, y + 1)
        - in(x + 3, y + 2)
        + 0.25 * in(x + 3, y + 3);
    c[2][3] = (- 0.5) * in(x, y)
        + 1.5 * in(x, y + 1)
        +  (-1.5) * in(x, y + 2)
        + 0.5 * in(x, y + 3)
        + 1.25 * in(x + 1, y)
        +  (-3.75) * in(x + 1, y + 1)
        +  3.75 * in(x + 1, y + 2)
        + (- 1.25) * in(x + 1, y + 3)
        - in(x + 2, y)
        +  3 * in(x + 2, y + 1)
        + (-3) * in(x + 2, y + 2)
        + in(x + 2, y + 3)
        +  0.25 * in(x + 3, y)
        +  (- 0.75) * in(x + 3, y + 1)
        + 0.75 * in(x + 3, y + 2)
        + (- 0.25) * in(x + 3, y + 3);
    c[3][0] = (- 0.5) * in(x, y + 1)
        + 1.5 * in(x + 1, y + 1)
        +  (- 1.5) * in(x + 2, y + 1)
        + 0.5 * in(x + 3, y + 1);
    c[3][1] = 0.25 * in(x, y)
        + (- 0.25) * in(x, y + 2)
        + (- 0.75) * in(x + 1, y)
        +  0.75 * in(x + 1, y + 2)
        +  0.75 * in(x + 2, y)
        + (- 0.75) * in(x + 2, y + 2)
        +  (- 0.25) * in(x + 3, y)
        + 0.25 * in(x + 3, y + 2);
    c[3][2] = (- 0.5) * in(x, y)
        + 1.25 * in(x, y + 1)
        - in(x, y + 2)
        + 0.25 * in(x, y + 3)
        + 1.5 * in(x + 1, y)
        +  (-3.75) * in(x + 1, y + 1)
        +  3 * in(x + 1, y + 2)
        + (- 0.75) * in(x + 1, y + 3)
        + (- 1.5) * in(x + 2, y)
        +  3.75 * in(x + 2, y + 1)
        + (-3) * in(x + 2, y + 2)
        + 0.75 * in(x + 2, y + 3)
        +  0.5 * in(x + 3, y)
        +  (- 1.25) * in(x + 3, y + 1)
        + in(x + 3, y + 2)
        + (- 0.25) * in(x + 3, y + 3);
    c[3][3] = 0.25 * in(x, y)
        + (- 0.75) * in(x, y + 1)
        +  0.75 * in(x, y + 2)
        + (-0.25) * in(x, y + 3)
        + (- 0.75) * in(x + 1, y)
        +  2.25 * in(x + 1, y + 1)
        +  (- 2.25) * in(x + 1, y + 2)
        + 0.75 * in(x + 1, y + 3)
        +  0.75 * in(x + 2, y)
        +  (-2.25) * in(x + 2, y + 1)
        + 2.25 * in(x + 2, y + 2)
        + (- 0.75) * in(x + 2, y + 3)
        + (- 0.25) * in(x + 3, y)
        +  0.75 * in(x + 3, y + 1)
        + (- 0.75) * in(x + 3, y + 2)
        + 0.25 * in(x + 3, y + 3);

    double result;
    for (int idx1 = 0; idx1 < 4; ++idx1)
    {
        for (int idx2 = 0; idx2 < 4; ++idx2)
        {
            result += c[idx1][idx2] * std::pow(x, idx1) * std::pow(y, idx2);
        }
     }
     if (result > 255) {
        return 255;
     }
     if (result < 0) {
        return 0;
     }

     return (int)round(result);

}

QImage Image::getTrimmedImage(const QImage &image, const QImage &screen, const QPoint &center, double scaleX, double scaleY)
{
    int size  = image.width();
    int trimWidth =  min(size, (int)round(screen.width() / scaleX));
    int trimHeight = min(size, (int)round(screen.height() / scaleY));
    int startX;
    int startY;

    if (trimWidth == size) {
        startX = -size / 2;
    } else {
        startX = (int)round((- screen.width() / 2.0 - center.x()) / (scaleX));
        if (- size / 2 > startX) {
            trimWidth -= (size / 2 - startX);
            startX = - size / 2;
        }

        if (startX + trimWidth > size / 2) {
            trimWidth = size / 2 - startX;
        }
    }

    if (trimHeight == size) {
        startY = size / 2;
    } else {
        startY = (int)round((screen.height() / 2.0 + center.y()) / (scaleY));
        if (size / 2 < startY) {
            trimHeight -= (size / 2  - startY);
            startY = size / 2;
        }

        if (startY - trimHeight < - size / 2) {
            trimHeight = startY - size / 2;
        }
    }

    QImage trimmedImage(image_.copy(linearTransformation(startX, 1, size / 2),
                                     linearTransformation(startY, -1,  size / 2),
                                     trimWidth, trimHeight));

    return trimmedImage;

}

void Image::showImage(QImage &screen, const QImage &image, const QPoint &shift)
{
    int centralX1 = screen.width() / 2;
    int centralY1 = screen.height() / 2;
    int centralX2 = image.width() / 2;
    int centralY2 = image.height() / 2;
    int x0 = shift.x();
    int y0 = shift.y();

    for (int y = -centralY1 + 1; y < centralY1 ; ++y) {
        if (((y - y0) > - centralY2) && ((y - y0) < centralY2)) {
            for (int x = - centralX1 + 1; x < centralX1; ++x) {
                if (((x - x0) > -centralX2) && ((x - x0) < centralX2)) {
                    screen.setPixel(linearTransformation(x, 1, centralX1), linearTransformation(y, -1, centralY1),
                                    image.pixel(linearTransformation(x - x0, 1, centralX2),
                                                linearTransformation(y - y0, -1, centralY2)));
                }
            }
        }
    }

}

void Image::biLinear(const QImage &image, QImage &backBuffer, double scaleX, double scaleY)
{
    int width = image.width();
    int height = image.height();

    int red[height][width];
    int green[height][width];
    int blue[height][width];
    ImageBuffer redBuffer((int *)red, width, height);
    ImageBuffer greenBuffer((int *)green, width, height);
    ImageBuffer blueBuffer((int *)blue, width, height);
    splitColors(image, redBuffer, greenBuffer, blueBuffer);

    int sizeX = backBuffer.width();
    int sizeY = backBuffer.height();

    int redResult[sizeY][sizeX];
    int greenResult[sizeY][sizeX];
    int blueResult[sizeY][sizeX];
    ImageBuffer outRed((int *)redResult, sizeX, sizeY);
    ImageBuffer outGreen((int *)greenResult, sizeX, sizeY);
    ImageBuffer outBlue((int *)blueResult, sizeX, sizeY);

    biLinearTransformation(redBuffer, outRed, scaleX, scaleY);
    biLinearTransformation(greenBuffer, outGreen, scaleX, scaleY);
    biLinearTransformation(blueBuffer, outBlue, scaleX, scaleY);
    joinColors(backBuffer, outRed, outGreen, outBlue);
}

void Image::splitColors(const QImage &image, ImageBuffer &red, ImageBuffer &green, ImageBuffer &blue)
{
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb color = image.pixel(x, y);
            red(x, y) = qRed(color);
            green(x, y) = qGreen(color);
            blue(x, y) = qBlue(color);
        }
    }

}

void Image::joinColors(QImage &image, const ImageBuffer &red, const ImageBuffer &green, const ImageBuffer blue)
{
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            image.setPixel(x, y, qRgb(red(x, y), green(x, y), blue(x, y)));
        }
    }

}

void Image::generateMipmap(QImage image, QImage &result)
{
    int size = image.width();

    int startX = 0;

    QImage next(size / 2, size / 2, QImage::Format_RGB888);
    for (; size; size /= 2) {
        for (int y = 0; y <  size; ++y)
        {
            for (int x = 0; x < size; ++x) {
                result.setPixel(startX + x, y, image.pixel(x, y));
            }
        }

        startX += size;
        reduceInHalf(image, next);
        image = next;
    }
}

void Image::getFromMipmap(const QImage &mipmap, QImage &image, int level)
{
    int size = mipmap.height();
    int position = 0;
    for (int i = 0; i <= level; ++i) {
        position += size;
        size /= 2;
    }

    image = mipmap.copy(position, 0, size, size);
}

int Image::getLevel(int realSize, int size)
{
    while (realSize < size) {
        return 0;
    }

    return (int)round(log(max(1, realSize - 1))) - (int)round(log(max(1, (size - 1))));
}

void Image::reduceInHalf(const QImage &image, QImage &result)
{
    for (int y = 1; y < image.height(); y += 2) {
        for (int x = 1; x < image.width(); x += 2) {
            QRgb color1 = image.pixel(x - 1, y - 1);
            QRgb color2 = image.pixel(x - 1, y);
            QRgb color3 = image.pixel(x, y - 1);
            QRgb color4 = image.pixel(x, y);
            result.setPixel(x / 2, y / 2, qRgb((int)round((qRed(color1) + qRed(color2) + qRed(color3) + qRed(color4)) / 4.0),
                                               (int)round((qGreen(color1) + qGreen(color2) + qGreen(color3) + qGreen(color4)) / 4.0),
                                               (int)round((qBlue(color1) + qBlue(color2) + qBlue(color3) + qBlue(color4)) / 4.0)));
        }
    }
}

int Image::calculateSize(int size, double scale)
{
    int step1 = (int)round(scale);
    int step2 = (int)round(scale);
    if (step1 == scale) {
        return (int)round(size * scale);
    }

    if (step1 < scale) {
        step2 = step1 + 1;
    } else {
        step2 = step1 - 1;
    }
    int period = (int)round((step2 - scale) / (scale - step1));

    return (step1 * period + step2) * (size / (period + 1)) + step1 * (size % (period + 1));
}

void Image::biLin(const QImage &image, QImage &backBuffer)
{
    int width = image.width();
    int height = image.height();

    int red[height][width];
    int green[height][width];
    int blue[height][width];
    ImageBuffer redBuffer((int *)red, width, height);
    ImageBuffer greenBuffer((int *)green, width, height);
    ImageBuffer blueBuffer((int *)blue, width, height);
    splitColors(image, redBuffer, greenBuffer, blueBuffer);

    int redResult[height][width];
    int greenResult[height][width];
    int blueResult[height][width];
    ImageBuffer outRed((int *)redResult, width, height);
    ImageBuffer outGreen((int *)greenResult, width, height);
    ImageBuffer outBlue((int *)blueResult, width, height);

    biLinearTransform(redBuffer, outRed);
    biLinearTransform(greenBuffer, outGreen);
    biLinearTransform(blueBuffer, outBlue);
    joinColors(backBuffer, outRed, outGreen, outBlue);
}

void Image::biLinearTransform(const ImageBuffer &in, ImageBuffer &out)
{
    for (int y = 0; y < (in.height - 1); ++y)
    {
        for (int x = 0; x < (in.width - 1); ++x)
        {
            double result = 0.25 * (in(x, y) + in(x, y + 1) + in(x + 1, y) + in(x + 1, y + 1));
            int res;
            if (result > 255) {
                res = 255;
            } else if (result < 0) {
                res = 0;
            } else {
                res = (int)round(result);
            }
            out(x, y) = res;
        }
    }

}

void Image::biLinearTransformation(const ImageBuffer &in, ImageBuffer &out, double scaleX, double scaleY)
{
    int interpolateOrder = 3;
    int stepX = (int)round(scaleX);
    int stepX1;
    int periodX;
    if ((scaleX - stepX) == 0) {
        stepX1 = stepX;
        periodX = numeric_limits<int>::max() - 1;
    } else {
        if (scaleX  > stepX) {
            stepX1 = stepX + 1;
        } else {
            stepX1 = stepX - 1;
        }
        periodX = 1 + (int)round((stepX1 - scaleX) / (scaleX - stepX));
    }

    int stepY = (int)round(scaleY);
    int stepY1;
    int periodY;
    if ((scaleY - stepY) == 0) {
        stepY1 = stepY;
        periodY = numeric_limits<int>::max() - 1;
    } else {
        if (stepY < scaleY) {
            stepY1 = stepY + 1;
        } else {
            stepY1 = stepY - 1;
        }
        periodY = 1 + (int)round((stepY1 - scaleY) / (scaleY - stepY));
    }

    int xCount = 0;
    int yCount = 0;

    for (int y = 0; y < (in.height - interpolateOrder); ++y)
    {
        int yLimit= (y % periodY)? stepY : stepY1;
        xCount = 0;
        for (int x = 0; x < (in.width - interpolateOrder); ++x)
        {
            int xLimit = (x % periodX)? stepX : stepX1;
            double c[interpolateOrder + 1][interpolateOrder + 1];
            c[0][0] = in(x + 1, y + 1);
            c[0][1] = (- 0.5) * in(x + 1, y)
                    + 0.5 * in(x + 1, y + 2);
                c[0][2] = in(x + 1, y)
                    + (- 2.5) * in(x + 1, y + 1)
                    + 2 * in(x + 1, y + 2)
                    + (- 0.5) * in(x + 1, y + 3);
                c[0][3] = (-0.5) * in(x + 1, y)
                    + 1.5 * in(x + 1, y + 1)
                    + (- 1.5) * in(x + 1, y + 2)
                    + 0.5 * in(x + 1, y + 3);
            c[1][0] = (-0.5) * in(x, y + 1)
                + 0.5 * in(x + 2, y + 1);
            c[1][1] = 0.25 * in(x, y)
                + (- 0.25) * in(x, y + 2)
                + (- 0.25) * in(x + 2, y)
                + 0.25 * in(x + 2, y + 2);
            c[1][2] = (- 0.5) * in(x, y)
                + 1.25 * in(x, y + 1)
                - in(x, y + 2)
                + 0.25 * in(x, y + 3)
                + 0.5 * in(x + 2, y)
                + (- 1.25) * in(x + 2, y + 1)
                + in(x + 2, y + 2)
                + (- 0.25) * in(x + 2, y + 3);
            c[1][2] = (- 0.5) * in(x, y)
                + 1.25 * in(x, y + 1)
                - in(x, y + 2)
                + 0.25 * in(x, y + 3)
                + 0.5 * in(x + 2, y)
                + (- 1.25) * in(x + 2, y + 1)
                + in(x + 2, y + 2)
                + (- 0.25) * in(x + 2, y + 3);
            c[1][3] = 0.25 * in(x, y)
                + (- 0.75) * in(x, y + 1)
                +  0.75 * in(x, y + 2)
                + (-0.25) * in(x, y + 3)
                + (-0.25) * in(x + 2, y)
                + 0.75 * in(x + 2, y + 1)
                + (- 0.75) * in(x + 2, y + 2)
                + 0.25 * in(x + 2, y + 3);
            c[2][0] = in(x, y + 1)
                + (- 2.5) * in(x + 1, y + 1)
                +  2 * in(x + 2, y + 1)
                + (- 0.5) * in(x + 3, y + 1);
            c[2][1] = (- 0.5) * in(x, y)
                + 0.5 * in(x, y + 2)
                +  1.25 * in(x + 1, y)
                + (-1.25) * in(x + 1, y + 2)
                - in(x + 2, y)
                + in(x + 2, y + 2)
                + 0.25 * in(x + 3, y)
                + (- 0.25) * in(x + 3, y + 2);
            c[2][2] = in(x, y)
                + (- 2.5) * in(x, y + 1)
                +  2 * in(x, y + 2)
                + (-0.5) * in(x, y + 3)
                + (- 2.5) * in(x + 1, y)
                +  6.25 * in(x + 1, y + 1)
                +  (- 5) * in(x + 1, y + 2)
                + 1.25 * in(x + 1, y + 3)
                +  2 * in(x + 2, y)
                +  (-5) * in(x + 2, y + 1)
                + 4 * in(x + 2, y + 2)
                - in(x + 2, y + 3)
                + (- 0.5) * in(x + 3, y)
                +  1.25 * in(x + 3, y + 1)
                - in(x + 3, y + 2)
                + 0.25 * in(x + 3, y + 3);
            c[2][3] = (- 0.5) * in(x, y)
                + 1.5 * in(x, y + 1)
                +  (-1.5) * in(x, y + 2)
                + 0.5 * in(x, y + 3)
                + 1.25 * in(x + 1, y)
                +  (-3.75) * in(x + 1, y + 1)
                +  3.75 * in(x + 1, y + 2)
                + (- 1.25) * in(x + 1, y + 3)
                - in(x + 2, y)
                +  3 * in(x + 2, y + 1)
                + (-3) * in(x + 2, y + 2)
                + in(x + 2, y + 3)
                +  0.25 * in(x + 3, y)
                +  (- 0.75) * in(x + 3, y + 1)
                + 0.75 * in(x + 3, y + 2)
                + (- 0.25) * in(x + 3, y + 3);
            c[3][0] = (- 0.5) * in(x, y + 1)
                + 1.5 * in(x + 1, y + 1)
                +  (- 1.5) * in(x + 2, y + 1)
                + 0.5 * in(x + 3, y + 1);
            c[3][1] = 0.25 * in(x, y)
                + (- 0.25) * in(x, y + 2)
                + (- 0.75) * in(x + 1, y)
                +  0.75 * in(x + 1, y + 2)
                +  0.75 * in(x + 2, y)
                + (- 0.75) * in(x + 2, y + 2)
                +  (- 0.25) * in(x + 3, y)
                + 0.25 * in(x + 3, y + 2);
            c[3][2] = (- 0.5) * in(x, y)
                + 1.25 * in(x, y + 1)
                - in(x, y + 2)
                + 0.25 * in(x, y + 3)
                + 1.5 * in(x + 1, y)
                +  (-3.75) * in(x + 1, y + 1)
                +  3 * in(x + 1, y + 2)
                + (- 0.75) * in(x + 1, y + 3)
                + (- 1.5) * in(x + 2, y)
                +  3.75 * in(x + 2, y + 1)
                + (-3) * in(x + 2, y + 2)
                + 0.75 * in(x + 2, y + 3)
                +  0.5 * in(x + 3, y)
                +  (- 1.25) * in(x + 3, y + 1)
                + in(x + 3, y + 2)
                + (- 0.25) * in(x + 3, y + 3);
            c[3][3] = 0.25 * in(x, y)
                + (- 0.75) * in(x, y + 1)
                +  0.75 * in(x, y + 2)
                + (-0.25) * in(x, y + 3)
                + (- 0.75) * in(x + 1, y)
                +  2.25 * in(x + 1, y + 1)
                +  (- 2.25) * in(x + 1, y + 2)
                + 0.75 * in(x + 1, y + 3)
                +  0.75 * in(x + 2, y)
                +  (-2.25) * in(x + 2, y + 1)
                + 2.25 * in(x + 2, y + 2)
                + (- 0.75) * in(x + 2, y + 3)
                + (- 0.25) * in(x + 3, y)
                +  0.75 * in(x + 3, y + 1)
                + (- 0.75) * in(x + 3, y + 2)
                + 0.25 * in(x + 3, y + 3);
            for (int k = 0; k < yLimit; ++k)
            {
                double yMult = (double)k / yLimit;

                for (int l = 0; l < xLimit; ++ l)
                {
                    double xMult = (double)l / xLimit;
                    double result = 0;

                    for (int idx1 = 0; idx1 < interpolateOrder + 1; ++idx1)
                    {
                        for (int idx2 = 0; idx2 < interpolateOrder + 1; ++idx2)
                        {
                            result += c[idx1][idx2] * std::pow(xMult, idx1) * std::pow(yMult, idx2);
                        }
                    }
                    int res;
                    if (result > 255) {
                        res = 255;
                    } else if (result < 0) {
                        res = 0;
                    } else {
                        res = (int)round(result);
                    }
                    if ((xCount + l >= out.width) || (yCount + k >= out.height)) {
                    }

                    out(xCount + l, yCount + k) = res;

                }
            }
            xCount += xLimit;
        }
        yCount += yLimit;
    }
}

int Image::linearTransformation(int x, int alpha, int shift)
{
    return x * alpha + shift;
}

const Image::Parameters &Image::getParameters() const
{
    return parameters_;
}


void Image::changeScale(double mult)
{
    parameters_.setScaleX(parameters_.scaleX() * mult);
    parameters_.setScaleY(parameters_.scaleY() * mult);

    if (mult > 0) {
        if (parameters_.scaleX() == 0) {
            parameters_.setScaleX(0.1 * mult);
        }
        if (parameters_.scaleY() == 0) {
            parameters_.setScaleY(0.1 * mult);
        }
    }

    emit updated();
}

void decreaseScale(double mult);

void Image::setCenter(const QPoint &center)
{
    parameters_.setCenter(center);

    emit updated();
}
void Image::setX(int x)
{
    parameters_.setX(x);

    emit updated();
}
void Image::setY(int y)
{
    parameters_.setY(y);

    emit updated();
}
void Image::setScaleX(double scale)
{
    parameters_.setScaleX(scale);

    emit updated();
}
void Image::setScaleY(double scale)
{
    parameters_.setScaleY(scale);

    emit updated();
}

void Image::setMode(int mode)
{
    parameters_.setMode(static_cast<Parameters::Mode>(mode));

    emit updated();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QRadioButton>
#include <QVBoxLayout>

#include "drawwidget.h"
#include "image.h"
#include "../Common/controller.h"
#include "../Common/doublecontroller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const QImage &image, QWidget *parent = 0);

public slots:
    void updateControllers();
private slots:
    void loadXml();
    void saveXml();
    void savePng();

private:
    static const int MAX_X_ = 1000;
    static const int MAX_Y_ = 1000;
    static const float MIN_SCALE_ = 0;
    static const int MIN_X_ = -1000;
    static const int MIN_Y_ = -1000;
    static const float MAX_SCALE_ = 10;
    static const int DEFAULT_X_ = 0;
    static const int DEFAULT_Y_ = 0;
    static const int DEFAULT_SCALE_ = 1;
    std::map<std::string, Controller *> parameters_;

    std::auto_ptr<QGroupBox> mainGroupBox_;
    std::auto_ptr<QVBoxLayout> mainLayout_;

    std::auto_ptr<QHBoxLayout> controllersLayout_;
    std::auto_ptr<QGroupBox> controllersGroupBox_;
    std::auto_ptr<Controller> xController_;
    std::auto_ptr<Controller> yController_;
    std::auto_ptr<DoubleController> scaleXController_;
    std::auto_ptr<DoubleController> scaleYController_;

    std::auto_ptr<QButtonGroup> modeButtons_;
    std::auto_ptr<QRadioButton> simpleModeButton_;
    std::auto_ptr<QRadioButton> biLinearMode_;
    std::auto_ptr<QRadioButton> mipMappingMode_;

    std::auto_ptr<QGridLayout> drawLayout_;
    std::auto_ptr<QFrame> drawFrame_;
    std::auto_ptr<DrawWidget> drawWidget_;
    std::auto_ptr<Image> image_;

    template <typename T> std::string numberToString(T pNumber);

};

#endif // MAINWINDOW_H

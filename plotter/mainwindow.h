#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGroupBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QVBoxLayout>

#include <map>
#include <memory>
#include <string>

#include "drawwidget.h"
#include "ovalofCassini.h"
#include "../Common/controller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);

public slots:
    void updateControllers();
private slots:
    void loadXml();
    void saveXml();
    void savePng();

private:
    static const int MAX_X_ = 1000;
    static const int MAX_Y_ = 1000;
    static const int MAX_R_ = 1000;
    static const int MIN_X_ = -1000;
    static const int MIN_Y_ = -1000;
    static const int MIN_R_ = -1000;
    static const int DEFAULT_X1_ = -10;
    static const int DEFAULT_X2_ = 10;
    static const int DEFAULT_Y1_ = 0;
    static const int DEFAULT_Y2_ = 0;
    static const int DEFAULT_R_ = 10;
    std::map<std::string, Controller *> parameters_;

    std::auto_ptr<QGroupBox> mainGroupBox_;
    std::auto_ptr<QVBoxLayout> mainLayout_;

    std::auto_ptr<QHBoxLayout> controllersLayout_;
    std::auto_ptr<QGroupBox> controllersGroupBox_;
    std::auto_ptr<Controller> x1Controller_;
    std::auto_ptr<Controller> x2Controller_;
    std::auto_ptr<Controller> y1Controller_;
    std::auto_ptr<Controller> y2Controller_;
    std::auto_ptr<Controller> rController_;
    std::auto_ptr<QMenu> fileMenu_;

    std::auto_ptr<QGridLayout> drawLayout_;
    std::auto_ptr<QFrame> drawFrame_;
    std::auto_ptr<DrawWidget> drawWidget_;
    std::auto_ptr<OvalOfCassini> oval_;

    template <typename T> std::string numberToString(T pNumber);
};

#endif // MAINWINDOW_H

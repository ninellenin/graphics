#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QObject>

#include "controller.h"
#include "drawwidget.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void loadXml();
    void saveXml();
    void savePng();

private:
    const int MAX_WIDTH_ = 10000;
    const int MAX_HEIGHT_ = 10000;
    const int MIN_HEIGHT_ = -10000;
    const int MIN_WIDTH_ = -10000;

    std::auto_ptr<Circle> circle_;
    std::auto_ptr<DrawWidget> drawWidget_;

    std::auto_ptr<QGroupBox> mainGroupBox_;
    std::auto_ptr<QHBoxLayout> mainLayout_;
    std::auto_ptr<QVBoxLayout> controllersLayout_;
    std::auto_ptr<QGridLayout> drawLayout_;
    std::auto_ptr<Controller> xController_;
    std::auto_ptr<Controller> yController_;
    std::auto_ptr<Controller> rController_;
    std::auto_ptr<QGroupBox> controllersGroupBox_;
    std::auto_ptr<QFrame> drawFrame_;
    std::auto_ptr<QMenu> fileMenu_;

    void on_XSlider_Moved(int position);

};

#endif // MAINWINDOW_H

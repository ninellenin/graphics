#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QCheckBox>
#include <QMainWindow>

#include "drawwidget.h"
#include "font.h"
#include "../Common/controller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const std::string &xmlFile, QWidget *parent = 0);

public slots:
    void updateControllers();
private slots:
    void loadXml();
    void saveXml();
    void savePng();

private:
    static const int MAX_X_ = 1000;
    static const int MAX_Y_ = 1000;
    static const int MIN_SCALE_ = -1000;
    static const int MIN_X_ = -1000;
    static const int MIN_Y_ = -1000;
    static const int MAX_SCALE_ = 1000;
    static const int DEFAULT_X_ = 0;
    static const int DEFAULT_Y_ = 0;
    static const int DEFAULT_SCALE_ = 10;
    std::map<std::string, Controller *> parameters_;

    std::auto_ptr<QGroupBox> mainGroupBox_;
    std::auto_ptr<QVBoxLayout> mainLayout_;

    std::auto_ptr<QVBoxLayout> checkBoxesLayout_;
    std::auto_ptr<QGroupBox> checkBoxesGroupBox_;
    std::auto_ptr<QCheckBox> drawFillingCheckBox_;
    std::auto_ptr<QCheckBox> drawOutlineCheckBox_;
    std::auto_ptr<QHBoxLayout> controllersLayout_;
    std::auto_ptr<QGroupBox> controllersGroupBox_;
    std::auto_ptr<Controller> xController_;
    std::auto_ptr<Controller> yController_;
    std::auto_ptr<Controller> scaleController_;
    std::auto_ptr<QMenu> fileMenu_;

    std::auto_ptr<QGridLayout> drawLayout_;
    std::auto_ptr<QFrame> drawFrame_;
    std::auto_ptr<DrawWidget> drawWidget_;
    std::auto_ptr<Font> font_;

    template <typename T> std::string numberToString(T pNumber);
    std::vector<Font::Beziers> getCurves(std::string xmlFile);

};

#endif // MAINWINDOW_H

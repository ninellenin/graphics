#include "mainwindow.h"

#include <limits>
#include <sstream>
#include <vector>

#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

#include <font.h>
#include <xmlparser.h>
#include "../Common/myexception.h"

using std::string;
using std::vector;

MainWindow::MainWindow(const string &xmlFile, QWidget *parent) :
    QMainWindow(parent),
    mainGroupBox_(new QGroupBox()),
    controllersLayout_(new QHBoxLayout()),
    checkBoxesLayout_(new QVBoxLayout()),
    controllersGroupBox_(new QGroupBox()),
    xController_(new Controller("x", MIN_X_, MAX_X_)),
    yController_(new Controller("y", MIN_Y_, MAX_Y_)),
    scaleController_(new Controller("scale", MIN_SCALE_, MAX_SCALE_)),
    drawFrame_(new QFrame()),
    drawLayout_(new QGridLayout()),
    fileMenu_(new QMenu("File")),
    drawFillingCheckBox_(new QCheckBox("Filling")),
    drawOutlineCheckBox_(new QCheckBox("Outline")),
    checkBoxesGroupBox_(new QGroupBox())
{
    vector<Font::Beziers> beziers = getCurves(xmlFile);
    font_.reset(new Font(beziers));

    this->resize(500, 500);
    mainLayout_.reset(new QVBoxLayout(mainGroupBox_.get()));
    this->setCentralWidget(mainGroupBox_.get());
    drawWidget_.reset(new DrawWidget(this, font_.get()));
    drawFrame_->setLayout(drawLayout_.get());
    drawLayout_->addWidget(drawWidget_.get());

    controllersGroupBox_->setLayout(controllersLayout_.get());
    controllersGroupBox_->layout()->addWidget(xController_.get());
    controllersGroupBox_->layout()->addWidget(yController_.get());
    controllersGroupBox_->layout()->addWidget(scaleController_.get());

    checkBoxesGroupBox_->setLayout(checkBoxesLayout_.get());
    checkBoxesGroupBox_->layout()->addWidget(drawFillingCheckBox_.get());
    checkBoxesGroupBox_->layout()->addWidget(drawOutlineCheckBox_.get());
    controllersLayout_.get()->setAlignment(Qt::AlignCenter);
    controllersLayout_.get()->setAlignment(Qt::AlignCenter);
    mainLayout_->addWidget(drawFrame_.get());
    mainLayout_->addWidget(controllersGroupBox_.get());
    controllersLayout_->addWidget(checkBoxesGroupBox_.get());
    xController_.get()->changeValue(DEFAULT_X_);
    yController_.get()->changeValue(DEFAULT_Y_);
    scaleController_.get()->changeValue(DEFAULT_SCALE_);

    QAction *load = fileMenu_->addAction("Load .xml");
    QAction *saveXml = fileMenu_->addAction("Save .xml");
    QAction *savePng = fileMenu_->addAction("Save .png");
    this->menuBar()->addMenu(fileMenu_.get());

    connect(load, SIGNAL(triggered()), this, SLOT(loadXml()));
    connect(saveXml, SIGNAL(triggered()), this, SLOT(saveXml()));
    connect(savePng, SIGNAL(triggered()), this, SLOT(savePng()));

    connect(xController_.get(), SIGNAL(valueChanged(int)), font_.get(), SLOT(setX(int)));
    connect(yController_.get(), SIGNAL(valueChanged(int)), font_.get(), SLOT(setY(int)));
    connect(scaleController_.get(), SIGNAL(valueChanged(int)), font_.get(), SLOT(setScale(int)));
    connect(drawFillingCheckBox_.get(), SIGNAL(stateChanged(int)), font_.get(), SLOT(setFilling(int)));
    connect(drawOutlineCheckBox_.get(), SIGNAL(stateChanged(int)), font_.get(), SLOT(setOutline(int)));
    connect(font_.get(), SIGNAL(updated()), this, SLOT(updateControllers()));

    parameters_["x"] = xController_.get();
    parameters_["y"] = yController_.get();
    parameters_["scale"] = scaleController_.get();

    updateControllers();
    drawWidget_->repaint();

}

vector<Font::Beziers> MainWindow::getCurves(string xmlFile)
{
    vector<Font::Beziers> curves;
    vector<Font::Point> points;
    vector<bool> onCurveValues;

    QString xmlFileName(xmlFile.c_str());
    QFile file(xmlFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        throw MyException(file.errorString().toStdString());
    }

    XmlParser parser;
    vector<XmlParser::Parameter> values = parser.read(file.readAll().data());


    for( vector<XmlParser::Parameter>::iterator it = values.begin(); it != values.end(); ) {
        if (it->first.compare("number") == 0) {
            ++it;
            Font::Point point;
            bool onCurve;
            bool setX = false, setY = false, setCurve = false;

            while (!setX || !setY || !setCurve) {
                if (it->first.compare("x") == 0) {
                    setX = true;
                    point.setX(atoi((it->second).c_str()));
                } else if (it->first.compare("y") == 0) {
                    setY = true;
                    point.setY(atoi((it->second).c_str()));
                } else if (it->first.compare("oncurve") == 0) {
                    setCurve = true;
                    if (it->second.compare("true") == 0) {
                        onCurve = true;
                    } else if (it->second.compare("false") == 0) {
                        onCurve = false;
                    }
                }
                ++it;
            }
            points.push_back(point);
            if (onCurve) {
                onCurveValues.push_back(true);
            } else {
                onCurveValues.push_back(false);
            }
        } else {
            ++it;
        }
    }

    points.push_back(points.front());
    onCurveValues.push_back(true);
    vector<Font::Point>::iterator last = points.end();
    --last;

    for (vector<Font::Point>::iterator it = points.begin(); it != last;)
    {
        Font::Point p1 = *it;
        Font::Point p2;
        Font::Point p3;

        if (onCurveValues[it - points.begin() + 1]) {
            p2 = p1;
            p3 = *(++it);
        } else {
            p2 = *(++it);
            p3 = *(++it);
        }
        curves.push_back(Font::Beziers(p1, p2, p3));
    }

    return curves;

}

void MainWindow::updateControllers()
{
    Font::Parameters params = font_->getParameters();
    xController_->changeValue(params.x());
    yController_->changeValue(params.y());
    scaleController_->changeValue(params.scale());
    drawFillingCheckBox_->setCheckState((params.filling()) ? Qt::Checked : Qt::Unchecked);
    drawOutlineCheckBox_->setCheckState((params.outline()) ? Qt::Checked : Qt::Unchecked);

}

void MainWindow::loadXml()
{
   /* QString fileName_XML = QFileDialog::getOpenFileName(this, tr("Open File"), tr("/home/code/"), tr("XML(*.xml)"));
    QFile file(fileName_XML);
    if (!file.open(QIODevice::ReadOnly)) {
        throw MyException(file.errorString().toStdString());
    }

    XmlParser parser;
    map<string, string> default_values;
    map<string, string> values = parser.read(file.readAll().data(), default_values);
    map<string, string>::iterator position;

    for(map<string, Controller *>::iterator it = parameters_.begin(); it != parameters_.end(); ++it) {
        if ((position = values.find((*it).first)) != values.end()) {
            it->second->changeValue(atoi((position->second).c_str()));
        }
    }*/

    return;
}
void MainWindow::savePng()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr("/home/code/"));

    QImage image(drawWidget_->width(), drawWidget_->height(), QImage::Format_RGB888);
    font_->Draw(&image);
    image.save(fileName +".png");

    return;

}
void MainWindow::saveXml()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr("/home/code/"), tr("XML(*.xml)"));
    XmlParser parser;
    map <string, string> values;

    for (map<string, Controller *>::iterator it = parameters_.begin(); it != parameters_.end(); ++it) {
        values[it->first] = numberToString((it->second)->getValue());
    }
    string number;
    values["PanelWidth"] = numberToString(drawWidget_->width());
    values["PanelHeight"] = numberToString(drawWidget_->height());
    parser.write(fileName, &values);

    return;

}

template <typename T> string MainWindow::numberToString(T pNumber)
{
    std::ostringstream oOStrStream;
    oOStrStream << pNumber;

    return oOStrStream.str();
}

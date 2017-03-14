#include "mainwindow.h"

#include <cstdlib>
#include <sstream>

#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QMenu>
#include <QMenuBar>

#include "../Common/myexception.h"
#include "../Common/xmlparser.h"

using std::map;
using std::string;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mainGroupBox_(new QGroupBox()),
    controllersLayout_(new QHBoxLayout()),
    controllersGroupBox_(new QGroupBox()),
    x1Controller_(new Controller("x1", MIN_X_, MAX_X_)),
    x2Controller_(new Controller("x2", MIN_X_, MAX_X_)),
    y1Controller_(new Controller("y1", MIN_Y_, MAX_Y_)),
    y2Controller_(new Controller("y2", MIN_Y_, MAX_Y_)),
    rController_(new Controller("r", MIN_R_, MAX_R_)),
    drawFrame_(new QFrame()),
    drawLayout_(new QGridLayout()),
    fileMenu_(new QMenu("File")),
    oval_(new OvalOfCassini())
{
    this->resize(500, 500);
    mainLayout_.reset(new QVBoxLayout(mainGroupBox_.get()));
    this->setCentralWidget(mainGroupBox_.get());
    drawWidget_.reset(new DrawWidget(this, oval_.get()));
    drawFrame_->setLayout(drawLayout_.get());
    drawLayout_->addWidget(drawWidget_.get());

    controllersGroupBox_->setLayout(controllersLayout_.get());
    controllersGroupBox_->layout()->addWidget(x1Controller_.get());
    controllersGroupBox_->layout()->addWidget(x2Controller_.get());
    controllersGroupBox_->layout()->addWidget(y1Controller_.get());
    controllersGroupBox_->layout()->addWidget(y2Controller_.get());
    controllersGroupBox_->layout()->addWidget(rController_.get());
    controllersLayout_.get()->setAlignment(Qt::AlignCenter);
    mainLayout_->addWidget(drawFrame_.get());
    mainLayout_->addWidget(controllersGroupBox_.get());
    x1Controller_.get()->changeValue(DEFAULT_X1_);
    x2Controller_.get()->changeValue(DEFAULT_X2_);
    y1Controller_.get()->changeValue(DEFAULT_Y1_);
    y2Controller_.get()->changeValue(DEFAULT_Y2_);
    rController_.get()->changeValue(DEFAULT_R_);

    QAction *load = fileMenu_->addAction("Load .xml");
    QAction *saveXml = fileMenu_->addAction("Save .xml");
    QAction *savePng = fileMenu_->addAction("Save .png");
    this->menuBar()->addMenu(fileMenu_.get());

    connect(load, SIGNAL(triggered()), this, SLOT(loadXml()));
    connect(saveXml, SIGNAL(triggered()), this, SLOT(saveXml()));
    connect(savePng, SIGNAL(triggered()), this, SLOT(savePng()));

    connect(x1Controller_.get(), SIGNAL(valueChanged(int)), oval_.get(), SLOT(setX1(int)));
    connect(x2Controller_.get(), SIGNAL(valueChanged(int)), oval_.get(), SLOT(setX2(int)));
    connect(y1Controller_.get(), SIGNAL(valueChanged(int)), oval_.get(), SLOT(setY1(int)));
    connect(y2Controller_.get(), SIGNAL(valueChanged(int)), oval_.get(), SLOT(setY2(int)));
    connect(x2Controller_.get(), SIGNAL(valueChanged(int)), oval_.get(), SLOT(setX2(int)));
    connect(rController_.get(), SIGNAL(valueChanged(int)), oval_.get(), SLOT(setR(int)));
    connect(oval_.get(), SIGNAL(updated()), this, SLOT(updateControllers()));

    parameters_["x1"] = x1Controller_.get();
    parameters_["y1"] = y1Controller_.get();
    parameters_["x2"] = x2Controller_.get();
    parameters_["y2"] = y2Controller_.get();
    parameters_["r"] = rController_.get();

    drawWidget_->repaint();
    updateControllers();

}

void MainWindow::updateControllers()
{
    OvalOfCassini::Parameters params = oval_->getParameters();
    x1Controller_->changeValue(params.getX1());
    x2Controller_->changeValue(params.getX2());
    y1Controller_->changeValue(params.getY1());
    y2Controller_->changeValue(params.getY2());
    rController_->changeValue(params.getR());
}

void MainWindow::loadXml()
{
    QString fileName_XML = QFileDialog::getOpenFileName(this, tr("Open File"), tr("/home/code/"), tr("XML(*.xml)"));
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
    }

    return;
}
void MainWindow::savePng()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr("/home/code/"));

    QImage image(drawWidget_->width(), drawWidget_->height(), QImage::Format_RGB888);
    oval_->Draw(&image);
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


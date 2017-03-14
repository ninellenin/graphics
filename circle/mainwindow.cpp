#include "mainwindow.h"

#include <map>
#include <string>

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

#include "controller.h"
#include "myexception.h"
#include "xmlparser.h"

using std::map;
using std::min;
using std::string;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    circle_(new Circle(0, 0, 10)),
    drawFrame_(new QFrame()),
    mainGroupBox_(new QGroupBox(this)),
    controllersLayout_(new QVBoxLayout()),
    drawLayout_(new QGridLayout()),
    xController_(new Controller("X", MIN_WIDTH_, MAX_WIDTH_)),
    yController_(new Controller("Y", MIN_HEIGHT_, MAX_HEIGHT_)),
    rController_(new Controller("Radius", 10, min(MAX_WIDTH_, MAX_HEIGHT_))),
    controllersGroupBox_(new QGroupBox("Controllers")),
    fileMenu_(new QMenu("File"))
{
    this->resize(500, 300);
    drawWidget_.reset(new DrawWidget(this, circle_.get()));
    drawFrame_->setLayout(drawLayout_.get());
    drawLayout_->addWidget(drawWidget_.get());

    mainLayout_.reset(new QHBoxLayout(mainGroupBox_.get()));
    this->setCentralWidget(mainGroupBox_.get());

    controllersGroupBox_->setLayout(controllersLayout_.get());
    controllersGroupBox_->layout()->addWidget(xController_.get());
    controllersGroupBox_->layout()->addWidget(yController_.get());
    controllersGroupBox_->layout()->addWidget(rController_.get());
    controllersLayout_.get()->setAlignment(Qt::AlignTop);
    mainLayout_->addWidget(drawFrame_.get());
    mainLayout_->addWidget(controllersGroupBox_.get());
    QAction *load = fileMenu_->addAction("Load .xml");
    QAction *saveXml = fileMenu_->addAction("Save .xml");
    QAction *savePng = fileMenu_->addAction("Save .png");
    this->menuBar()->addMenu(fileMenu_.get());

    connect(load, SIGNAL(triggered()), this, SLOT(loadXml()));
    connect(saveXml, SIGNAL(triggered()), this, SLOT(saveXml()));
    connect(savePng, SIGNAL(triggered()), this, SLOT(savePng()));

    connect(xController_.get(), SIGNAL(valueChanged(int)), drawWidget_.get(), SLOT(setX(int)));
    connect(yController_.get(), SIGNAL(valueChanged(int)), drawWidget_.get(), SLOT(setY(int)));
    connect(rController_.get(), SIGNAL(valueChanged(int)), drawWidget_.get(), SLOT(setRadius(int)));
    connect(circle_.get(), SIGNAL(updated()), drawWidget_.get(), SLOT(repaint()));

    drawWidget_->repaint();
}


MainWindow::~MainWindow()
{
}

void MainWindow::loadXml()
{
    QString fileName_XML = QFileDialog::getOpenFileName(this, tr("Open File"), tr("/home/code/"), tr("XML(*.xml)"));
    QFile file(fileName_XML);
    if (!file.open(QIODevice::ReadOnly)) {
        throw MyException(file.errorString().toStdString());
    }

    XmlParser parser;
    map<string, int> values = parser.read(file.readAll().data());

    xController_->changeValue(values["PosX"]);
    yController_->changeValue(values["PosY"]);
    rController_->changeValue(values["R"]);

    return;
}
void MainWindow::savePng()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr("/home/code/"));

    QImage image(drawWidget_->width(), drawWidget_->height(), QImage::Format_RGB888);
    circle_->Draw(&image);
    image.save(fileName +".png");

    return;

}
void MainWindow::saveXml()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), tr("/home/code/"), tr("XML(*.xml)"));
    XmlParser parser;
    map <string, int> values;
    values["PosX"] = xController_->getValue();
    values["PosY"] = yController_->getValue();
    values["R"] = rController_->getValue();
    values["PanelWidth"] = drawWidget_->width();
    values["PanelHeight"] = drawWidget_->height();

    parser.write(fileName, &values);

    return;

}

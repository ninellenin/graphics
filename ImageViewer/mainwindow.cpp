#include "mainwindow.h"

#include "../Common/myexception.h"

using std::string;

MainWindow::MainWindow(const QImage &image, QWidget *parent) :
    QMainWindow(parent),
    mainGroupBox_(new QGroupBox()),
    controllersLayout_(new QHBoxLayout()),
    controllersGroupBox_(new QGroupBox()),
    xController_(new Controller("x", MIN_X_, MAX_X_)),
    yController_(new Controller("y", MIN_Y_, MAX_Y_)),
    scaleXController_(new DoubleController("scaleX", MIN_SCALE_, MAX_SCALE_)),
    scaleYController_(new DoubleController("scaleY", MIN_SCALE_, MAX_SCALE_)),
    drawFrame_(new QFrame()),
    drawLayout_(new QGridLayout()),
    image_(new Image(image)),
    simpleModeButton_(new QRadioButton("0")),
    biLinearMode_(new QRadioButton("1")),
    mipMappingMode_(new QRadioButton("2")),
    modeButtons_(new QButtonGroup())
{
    this->resize(500, 500);
    mainLayout_.reset(new QVBoxLayout(mainGroupBox_.get()));
    this->setCentralWidget(mainGroupBox_.get());
    drawWidget_.reset(new DrawWidget(this, image_.get()));
    drawFrame_->setLayout(drawLayout_.get());
    drawLayout_->addWidget(drawWidget_.get());

    controllersGroupBox_->setLayout(controllersLayout_.get());
    controllersGroupBox_->layout()->addWidget(xController_.get());
    controllersGroupBox_->layout()->addWidget(yController_.get());
    controllersGroupBox_->layout()->addWidget(scaleXController_.get());
    controllersGroupBox_->layout()->addWidget(scaleYController_.get());

    modeButtons_.get()->addButton(simpleModeButton_.get(), 0);
    modeButtons_.get()->addButton(biLinearMode_.get(), 1);
    modeButtons_.get()->addButton(mipMappingMode_.get(), 2);

    controllersLayout_.get()->setAlignment(Qt::AlignCenter);
    mainLayout_->addWidget(drawFrame_.get());
    mainLayout_->addWidget(controllersGroupBox_.get());
    controllersLayout_->addWidget(simpleModeButton_.get());
    controllersLayout_->addWidget(biLinearMode_.get());
    controllersLayout_->addWidget(mipMappingMode_.get());
    xController_.get()->changeValue(DEFAULT_X_);
    yController_.get()->changeValue(DEFAULT_Y_);
    scaleXController_.get()->changeValue(DEFAULT_SCALE_);
    scaleYController_.get()->changeValue(DEFAULT_SCALE_);

    connect(xController_.get(), SIGNAL(valueChanged(int)), image_.get(), SLOT(setX(int)));
    connect(yController_.get(), SIGNAL(valueChanged(int)), image_.get(), SLOT(setY(int)));
    connect(scaleXController_.get(), SIGNAL(valueChanged(double)), image_.get(), SLOT(setScaleX(double)));
    connect(scaleYController_.get(), SIGNAL(valueChanged(double)), image_.get(), SLOT(setScaleY(double)));
    connect(image_.get(), SIGNAL(updated()), this, SLOT(updateControllers()));
    connect(modeButtons_.get(), SIGNAL(buttonClicked(int)), image_.get(), SLOT(setMode(int)));

    updateControllers();
    drawWidget_->repaint();

}

void MainWindow::updateControllers()
{
    Image::Parameters params = image_->getParameters();
    xController_->changeValue(params.x());
    yController_->changeValue(params.y());
    scaleXController_->changeValue(params.scaleX());
    scaleYController_->changeValue(params.scaleY());
}

void MainWindow::loadXml()
{
}

void MainWindow::saveXml()
{
}

void MainWindow::savePng()
{

}

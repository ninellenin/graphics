#include "controller.h"

Controller::Controller(const QString& name, int min, int max) :
    label_(new QLabel(name, this, 0)),
    spinBox_(new QSpinBox(this)),
    slider_(new QSlider(Qt::Horizontal, this)),
    vBoxLayout_(new QVBoxLayout(this)),
    hBoxLayout_(new QHBoxLayout())
{

    hBoxLayout_->addWidget(label_.get());
    hBoxLayout_->addWidget(spinBox_.get());
    vBoxLayout_->addLayout(hBoxLayout_.get());
    vBoxLayout_->addWidget(slider_.get());

    spinBox_->setMinimum(min);
    slider_->setMinimum(min);
    spinBox_->setMaximum(max);
    slider_->setMaximum(max);
    connect(spinBox_.get(), SIGNAL(valueChanged(int)), slider_.get(), SLOT(setValue(int)));
    connect(slider_.get(), SIGNAL(valueChanged(int)), spinBox_.get(), SLOT(setValue(int)));
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    spinBox_->installEventFilter(this);
    slider_->installEventFilter(this);

    connect(slider_.get(), SIGNAL(valueChanged(int)), this, SLOT(handleValueChanged(int)));
    connect(spinBox_.get(), SIGNAL(valueChanged(int)), this, SLOT(handleValueChanged(int)));
}

void Controller::changeValue(int newValue)
{
    slider_->setValue(newValue);
}

Controller::~Controller()
{
}

void Controller::handleValueChanged(int value)
{
    emit valueChanged(value);
}

void Controller::setValue(int newValue)
{
    changeValue(newValue);
}

int Controller::getValue()
{
    return slider_->value();
}

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>

#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>

class Controller : public QGroupBox
{
    Q_OBJECT
public:
    Controller(const QString &name, int min, int max);
    void changeValue(int newValue);
    int getValue();
    ~Controller();
public slots:
    void handleValueChanged(int newValue);
    void setValue(int newValue);
signals:
    void valueChanged(int newValue);
protected:
private:
    std::auto_ptr<QLabel> label_;
    std::auto_ptr<QSpinBox> spinBox_;
    std::auto_ptr<QSlider> slider_;
    std::auto_ptr<QHBoxLayout> hBoxLayout_;
    std::auto_ptr<QVBoxLayout> vBoxLayout_;
};

#endif // CONTROLLER_H

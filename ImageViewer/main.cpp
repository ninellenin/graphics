#include <iostream>

#include <QApplication>
#include <QImage>

#include "mainwindow.h"
#include "../Common/myexception.h"

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "Argument filename is missed" << endl;

        return -1;
    }
    QApplication a(argc, argv);
    std::string file(argv[1]);
    QImage image(file.c_str());
    if (image.isNull()) {
        cerr << "Can't open image " << file << endl;

        return -1;
    }

    MainWindow w(image);
    w.show();

    return a.exec();
}

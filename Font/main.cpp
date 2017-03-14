#include <iostream>

#include <QApplication>

#include "mainwindow.h"

using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "Argument xml_filename is missed" << endl;

        return -1;
    }
    QApplication a(argc, argv);
    std::string xmlFile(argv[1]);
    MainWindow w(xmlFile);
    w.show();
    
    return a.exec();
}

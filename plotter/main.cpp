#include <cstdlib>
#include <iostream>
#include <istream>
#include <fstream>
#include <map>
#include <string>

#include <QApplication>

#include "mainwindow.h"
#include "ovalofCassini.h"
#include "../Common/myexception.h"
#include "../Common/xmlparser.h"

using std::cout;
using std::endl;
using std::fstream;
using std::map;
using std::string;

int main(int argc, char *argv[])
{
    if (argc > 1) {
        fstream stream;
        stream.open(argv[1]);

        cout << stream.is_open() << endl;
        char buf[1024] = {0};
        stream.read(buf, 1024);
        XmlParser parser;
        map <string, string> default_values;

        default_values["r"] = "10";
        default_values["x1"] = "-10";
        default_values["y1"] = "0";
        default_values["x2"] = "10";
        default_values["y2"] = "0";
        default_values["PanelWidth"] = "100";
        default_values["PanelHeight"] = "100";
        default_values["OutputPath"] = "out.png";
        map <string, string> values;

        try {
            values = parser.read(buf, default_values);
        } catch (MyException e) {
            cout << e.what() << endl;
        }

        int panelWidth = atoi(values["PanelWidth"].c_str());
        int panelHeight = atoi(values["PanelHeight"].c_str());
        QImage buffer(panelWidth, panelHeight, QImage::Format_RGB888);
        int x1 = atoi(values["x1"].c_str());
        int y1 = atoi(values["y1"].c_str());
        int x2 = atoi(values["x2"].c_str());
        int y2 = atoi(values["y2"].c_str());
        int r = atoi(values["r"].c_str());
        OvalOfCassini::Parameters parameters(x1, y1, x2, y2, r);
        OvalOfCassini oval(parameters);
        oval.Draw(&buffer);
        buffer.save(default_values["OutputPath"].c_str());
        stream.close();

        return 0;
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}

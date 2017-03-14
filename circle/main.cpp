#include <QApplication>

#include <istream>
#include <fstream>
#include <map>
#include <string>

#include "mainwindow.h"
#include "xmlparser.h"

using std::fstream;
using std::map;
using std::string;

int main(int argc, char *argv[])
{
    if (argc > 1) {
        fstream stream;
        stream.open(argv[1]);
        char buf[1024] = {0};
        stream.read(buf, 1024);
        XmlParser parser;
        map <string, int> values = parser.read(buf);

        QImage buffer(values["PanelWidth"], values["PanelHeight"], QImage::Format_RGB888);
        Circle circle(values["PosX"], values["PosY"], values["R"]);
        circle.Draw(&buffer);
        buffer.save("out.png");
        stream.close();
        return 0;
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

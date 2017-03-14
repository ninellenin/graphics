#include "xmlparser.h"

#include <QFile>
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "myexception.h"


XmlParser::XmlParser()
{
}

XmlParser::~XmlParser()
{
}

map<string, int> XmlParser::read(const char *text)
{
    QXmlStreamReader *reader_ = new QXmlStreamReader(text);
    map <string, int> values;

    while (!reader_->atEnd()) {
        QXmlStreamReader::TokenType token = reader_->readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader_->name() == "entry") {
                QXmlStreamAttributes attributes = reader_->attributes();
                reader_->readNext();
                if (attributes.hasAttribute("key")) {
                    string str = attributes.value("key").toString().toStdString();
                    int val = reader_->text().toString().toInt();
                    values.insert(std::pair<string, int>(str, val));
                }
                continue;
            }
        }
    }
    static map<string, int> default_values;
    default_values["R"] = 10;
    default_values["PosX"] = 0;
    default_values["PosY"] = 0;
    default_values["PanelWidth"] = 100;
    default_values["PanelHeight"] = 100;

    for (map<string, int>::iterator it = default_values.begin(); it != default_values.end(); ++it) {
        if (values.find((*it).first) == values.end()) {
            values.insert(*it);
        }
    }

    if (reader_->hasError()) {
        throw MyException(reader_->errorString().toStdString());
    }

    return values;
}


void XmlParser::write(QString &fileName, map<string, int> *values)
{
    QFile out(fileName);
    out.open(QIODevice::WriteOnly);
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QString("properties"));
    for (map<string, int>::iterator it = values->begin(); it != values->end(); ++it) {
        writer.writeStartElement(QString("entry"));
        string name = (*it).first;
        writer.writeAttribute(QString("key"), QString(name.c_str()));
        writer.writeCharacters(QString::number((*it).second));
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    out.close();
}

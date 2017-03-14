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

map<string, string> XmlParser::read(const char *text, map<string, string> &default_values)
{
    QXmlStreamReader *reader_ = new QXmlStreamReader(text);
    map <string, string> values;

    while (!reader_->atEnd()) {
        QXmlStreamReader::TokenType token = reader_->readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader_->name() == "entry") {
                QXmlStreamAttributes attributes = reader_->attributes();
                reader_->readNext();
                if (attributes.hasAttribute("key")) {
                    string str = attributes.value("key").toString().toStdString();
                    string val = reader_->text().toString().toStdString();
                    values.insert(std::pair<string, string>(str, val));
                }
                continue;
            }
        }
    }

    for (map<string, string>::iterator it = default_values.begin(); it != default_values.end(); ++it) {
        if (values.find((*it).first) == values.end()) {
            values.insert(*it);
        }
    }

    if (reader_->hasError()) {
        throw MyException(reader_->errorString().toStdString());
    }

    return values;
}


void XmlParser::write(QString &fileName, map<string, string> *values)
{
    QFile out(fileName);
    out.open(QIODevice::WriteOnly);
    QXmlStreamWriter writer(&out);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QString("properties"));
    for (map<string, string>::iterator it = values->begin(); it != values->end(); ++it) {
        writer.writeStartElement(QString("entry"));
        string name = (*it).first;
        writer.writeAttribute(QString("key"), QString(name.c_str()));
        writer.writeCharacters(QString(it->second.c_str()));
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    out.close();
}

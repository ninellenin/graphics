#include "xmlparser.h"

#include <QFile>
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "../Common/myexception.h"

XmlParser::XmlParser()
{
}

XmlParser::~XmlParser()
{
}

std::vector<XmlParser::Parameter> XmlParser::read(const char *text)
{
    QXmlStreamReader *reader_ = new QXmlStreamReader(text);
    std::vector<Parameter> values;

    while (!reader_->atEnd()) {
        QXmlStreamReader::TokenType token = reader_->readNext();
        if (token == QXmlStreamReader::StartElement) {
            QXmlStreamAttributes attributes = reader_->attributes();
            reader_->readNext();
            for (QVector<QXmlStreamAttribute>::iterator attr = attributes.begin(); attr != attributes.end(); ++attr)
            {
                values.push_back(std::pair<string, string>(attr->name().toString().toStdString(),
                                                            attr->value().toString().toStdString()));
            }
            continue;
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

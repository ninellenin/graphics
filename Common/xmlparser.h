#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <map>
#include <string>

#include <QString>

using std::map;
using std::string;

class XmlParser
{
public:
    XmlParser();
    ~XmlParser();
    map<string, string> read(const char *text, map<string, string> &default_values);
    void write(QString &fileName, map<string, string> *values);
};

#endif // XMLPARSER_H

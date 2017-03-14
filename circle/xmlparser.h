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
    map<string, int> read(const char *text);
    void write(QString &fileName, map<string, int> *values);
};

#endif // XMLPARSER_H

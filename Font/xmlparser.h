#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <map>
#include <string>
#include <vector>

#include <QString>

using std::map;
using std::string;

class XmlParser
{
public:
    typedef std::pair<string, string> Parameter;
    XmlParser();
    ~XmlParser();
    std::vector<Parameter> read(const char *text);
    void write(QString &fileName, map<string, string> *values);
};

#endif // XMLPARSER_H

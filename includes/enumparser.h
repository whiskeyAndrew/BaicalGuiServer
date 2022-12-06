#ifndef ENUMPARSER_H
#define ENUMPARSER_H
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QMap>
#include "GTypes.h"
#include <QStringList>

struct enumFromFile{
    QString name;
    QString comment;
};


class EnumParser
{
private:
    QFile *file;
public:
    EnumParser(QString fileName);
    QMap <QString, QMap<tUINT32,enumFromFile>> enums;
    void readEnumsFromFile();
};

#endif // ENUMPARSER_H

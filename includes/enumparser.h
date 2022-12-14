#ifndef ENUMPARSER_H
#define ENUMPARSER_H
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QMap>
#include "GTypes.h"
#include <QStringList>
#include <QHash>

struct enumFromFile{
    QString name;
    QString comment;
};

struct likeEnum{
    QString name;
    QMap<tUINT32,enumFromFile> enums;
};

class EnumParser
{
private:
    QFile* file;
public:
    EnumParser();
    //QHash <QString, QMap<tUINT32,enumFromFile>> enums;
    QList<likeEnum> enums;
    tBOOL readEnumsFromFile(QString fileName);
};

#endif // ENUMPARSER_H

#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QQueue>
#include <QDateTime>

class DebugLogger
{
private:
public:
    static void writeData(QString data);
};

#endif // DEBUGLOGGER_H

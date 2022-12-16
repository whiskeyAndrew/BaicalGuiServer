#ifndef TRACESTOTXT_H
#define TRACESTOTXT_H
#include <QThread>
#include <QMap>
#include "GTypes.h"
#include <QFile>
#include <QDateTime>
#include <QTextStream>


//Инклюд ради GUIData, надо будет переделать
#include "Trace.h"

class TraceWindow;
class TracesToText:public QThread{
public:
    TracesToText(QMap<tUINT32, GUIData>* traces, QString filePath, TraceWindow* newTraceWindow);
    QMap<tUINT32, GUIData>* data;
private:
    QString fileDirectory;
    TraceWindow* traceWindow;
    void run();
};

#endif // TRACESTOTXT_H

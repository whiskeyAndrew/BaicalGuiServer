#ifndef TRACESTOTXT_H
#define TRACESTOTXT_H
#include <QThread>
#include "GTypes.h"
#include <QFile>
#include <QDateTime>
#include <QTextStream>


//Инклюд ради GUIData, надо будет переделать
#include "Trace.h"

class TraceWindow;
class TracesToText:public QThread{
public:
    TracesToText(QList<GUIData>* traces, QString filePath, TraceWindow* newTraceWindow);
    QList<GUIData>* data;
private:
    QString fileDirectory;
    TraceWindow* traceWindow;
    void run();
};

#endif // TRACESTOTXT_H

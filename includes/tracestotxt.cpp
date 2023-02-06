#include "tracestotxt.h"
#include "TraceWindow/tracewindow.h"

//Класс пишет все трейсы из traceWindow в текстовик. Фильтры и енамы не используются при сохранении, пишется чистый текст с подставленными аргументами

TracesToText::TracesToText(QList<GUIData>* traces, QString filePath, TraceWindow* newTraceWindow, Trace* traceThread)
{
    fileDirectory = filePath;
    data = traces;
    traceWindow = newTraceWindow;
    tracesThread = traceThread;
}

void TracesToText::run()
{
    traceWindow->setActionStatusText("Started to write .txt");
    QFile file(fileDirectory);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
    for(int i=0;i<data->size();i++){
        tUINT32 percent = 25;
        if(i == data->size()/100*percent){
            traceWindow->setActionStatusText("Started to write .txt: " + QString::number(percent) +"%");
            percent+=25;
        }

        GUIData dataToFile = data->at(i);
        UniqueTraceData uniqueTraceData = tracesThread->getUniqueTraces().value(dataToFile.uniqueData.wID);

        QList<ArgsPosition> argsPosition;

        p7Time rawTime = tracesThread->countTraceTime(dataToFile.uniqueData);
        QString traceRow = tracesThread->formatVector(&uniqueTraceData,dataToFile.argsValue, &argsPosition);

        QString hour = QString::number(rawTime.dwHour);
        QString minutes = QString::number(rawTime.dwMinutes);
        QString seconds = QString::number(rawTime.dwSeconds);
        QString milisecs = QString::number(rawTime.dwMilliseconds);

        if(hour.length()==1){
            hour.insert(0,"0");
        }

        if(minutes.length()==1){
            minutes.insert(0,"0");
        }

        if(seconds.length()==1){
            seconds.insert(0,"0");
        }

        while(milisecs.length()!=3){
            milisecs.insert(0,"0");
        }

        out<<QString::number(dataToFile.uniqueData.dwSequence)+" " + hour+":"+minutes+":"+seconds+"."+milisecs+" " + traceRow+"\n";
    }
    traceWindow->setActionStatusText("Traces has been written to a file");
    file.close();
    delete data;
}

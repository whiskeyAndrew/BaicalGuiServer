#include "tracestotxt.h"
#include "TraceWindow/tracewindow.h"

//Класс пишет все трейсы из traceWindow в текстовик. Фильтры и енамы не используются при сохранении, пишется чистый текст с подставленными аргументами

TracesToText::TracesToText(QList<GUIData>* traces, QString filePath, TraceWindow* newTraceWindow)
{
    fileDirectory = filePath;
    data = traces;
    traceWindow = newTraceWindow;
}

void TracesToText::run()
{
    traceWindow->setActionStatusText("Started to write .txt");
    QFile file(fileDirectory);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
//    for(int i=1;i<data->size();i++){
//        tUINT32 percent = 25;
//        if(i == data->size()/100*percent){
//            traceWindow->setActionStatusText("Started to write .txt: " + QString::number(percent) +"%");
//            percent+=25;
//        }
//        GUIData dataToFile = data->at(i);
//        out<<QString::number(dataToFile.uniqueData.dwSequence)+" " + QString::number(dataToFile.time.dwHour)+":"+QString::number(dataToFile.time.dwMinutes)+":"+QString::number(dataToFile.time.dwSeconds)+"."+QString::number(dataToFile.time.dwMilliseconds)+" " + dataToFile.trace+"\n";
//    }
    traceWindow->setActionStatusText("Traces has been written to a file");
    file.close();
    delete data;
}

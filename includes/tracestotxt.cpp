#include "tracestotxt.h"
#include "TraceWindow/tracewindow.h"

TracesToText::TracesToText(QMap<tUINT32, GUIData> *traces, TraceWindow *newTraceWindow)
{
    data = traces;
    traceWindow = newTraceWindow;
}

void TracesToText::run()
{
    traceWindow->SetActionStatusText("Started to write .txt");
    QFile file(QString::number(QDateTime::currentMSecsSinceEpoch())+".txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
    for(int i=0;i<data->size();i++){
        tUINT32 percent = 25;
        if(i == data->size()/100*percent){
            traceWindow->SetActionStatusText("Started to write .txt: " + QString::number(percent) +"%");
            percent+=25;
        }
        GUIData dataToFile = data->value(i);
        out<<QString::number(dataToFile.sequence)+" "+ dataToFile.trace+"\n";
    }
    traceWindow->SetActionStatusText("Traces has been written to a file");
    file.close();
    delete data;
}

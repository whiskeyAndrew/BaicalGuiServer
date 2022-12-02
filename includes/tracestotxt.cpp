#include "tracestotxt.h"


TracesToText::TracesToText(QMap<tUINT32, GUIData> *traces)
{
    data = traces;
}

void TracesToText::run()
{
    QFile file(QString::number(QDateTime::currentMSecsSinceEpoch())+".txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
    for(int i=0;i<data->size();i++){
        GUIData dataToFile = data->value(i);
        out<<QString::number(dataToFile.sequence)+" "+ dataToFile.trace+"\n";
    }
    file.close();
    delete data;
}

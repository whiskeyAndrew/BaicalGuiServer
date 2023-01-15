#include "debuglogger.h"
#include <iostream>

void DebugLogger::writeData(QString data)
{
    if(false){
        return;
    }

    data = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")+ " " + data + "\n";
    QFile file("debugLogs.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        std::cout<<"error with debugLogs"<<std::endl;
        return;
    }
    QTextStream stream(&file);
    stream <<data;
    stream.flush();
    file.close();
}

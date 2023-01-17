#include "debuglogger.h"
#include <iostream>

//На слабых системах выедает всю производиловку
//Переписать

void DebugLogger::writeData(QString data)
{
    if(true){
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

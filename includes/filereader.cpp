//По каким-то неведомым мне причинам fread не мог читать нормально файл
//Вместо того чтобы читать файл оно читает какую-то хрень
//
#include "filereader.h"


FileReader::FileReader()
{    

}

void FileReader::run()
{
    TraceWindow* traceWindow;
    chunkHandler.start();
    //------------Открываем файл и читаем его в буфер----------//
    ReadFileData();
    //-------------Обработка----------------//
    HandlingChunks();
    //На данном этапе мы закончили читать файл, закрываем потоки
    this->~FileReader();

}

FileReader::~FileReader()
{
    chunkHandler.wait();
    this->quit();
}

bool FileReader::ReadFileData()
{
    QFile logFile(fileName);

    // Создаем объект класса QByteArray, куда мы будем считывать данные
    if (!logFile.open(QIODevice::ReadOnly)) // Проверяем, возможно ли открыть наш файл для чтения
        return 0; // если это сделать невозможно, то завершаем функцию
    data = logFile.readAll(); //считываем все данные с файла в объект data
}

void FileReader::setTraceWindow(TraceWindow *newTraceWindow)
{
    traceWindow = newTraceWindow;
    chunkHandler.setTraceWindow(traceWindow);

}

void FileReader::setFileName(QString newFileName)
{
    fileName = newFileName;
}



bool FileReader::HandlingChunks()
{
    bufferCursor = data.begin();
    memcpy(&fileHeader,bufferCursor,sizeof(sP7File_Header));
    bufferCursor+=sizeof(sP7File_Header);

    while(bufferCursor<data.end())
    {
        memcpy(&chunkSize,bufferCursor,sizeof(tUINT32));
        for(int i =0;i<chunkSize;i++)
        {
            dataVector.push_back(*bufferCursor);
            bufferCursor++;
        }
        chunkHandler.AppendChunksQueue(dataVector);
        dataVector.clear();
        chunkSize=0;
    }
    chunkHandler.setFileEnded(true);
    return true;
}

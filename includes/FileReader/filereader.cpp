//По каким-то неведомым мне причинам fread не мог читать нормально файл
//Вместо того чтобы читать файл оно читает какую-то хрень
//
#include "filereader.h"
#include "filechunkshandler.h"

FileReader::FileReader()
{    

}

void FileReader::run()
{
    fileChunksHandler->start();
    //------------Открываем файл и читаем его в буфер----------//
    ReadFileData();
    //-------------Обработка----------------//
    HandlingChunks();
    fileChunksHandler->setFileEnded(true);
    //this->~FileReader();

}

FileReader::~FileReader()
{

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
    fileChunksHandler = new FileChunksHandler();
    fileChunksHandler->SetTraceWindow(traceWindow);
}

void FileReader::setFileName(QString newFileName)
{
    fileName = newFileName;
}



bool FileReader::HandlingChunks()
{
    sP7File_Header fileHeader;
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
        fileChunksHandler->AppendChunks(dataVector);
        dataVector.clear();
        chunkSize=0;
    }
    //chunkHandler.setFileEnded(true);
    return true;
}

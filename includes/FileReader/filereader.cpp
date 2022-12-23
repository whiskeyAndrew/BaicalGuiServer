//По каким-то неведомым мне причинам fread не мог читать нормально файл
//Вместо того чтобы читать файл оно читает какую-то хрень
//
#include "filereader.h"

//файл в 2600kb отжирает 50мб оперативы
//надо будет посмотреть откуда такой жирный меморилик
FileReader::FileReader(QString fileName, TraceWindow* newTraceWindow)
{    
    file = new QFile(fileName);
    traceWindow = newTraceWindow;
}

void FileReader::run()
{    
    chunkHandler.setTraceWindow(traceWindow);
    chunkHandler.setNeedBackup(false);
    chunkHandler.start();
    //------------Открываем файл и читаем его в буфер----------//
    ReadFileData();
    //-------------Обработка----------------//
    HandlingChunks();
    //this->~FileReader();
    std::cout<<"------FileReader:: closing myself------"<<std::endl;
    this->quit();
}

FileReader::~FileReader()
{

}

bool FileReader::ReadFileData()
{
    // Создаем объект класса QByteArray, куда мы будем считывать данные
    if (!file->open(QIODevice::ReadOnly)){
        return 0;
    }
    data = file->readAll();

}


bool FileReader::HandlingChunks()
{

    sP7File_Header fileHeader;
    bufferCursor = data.begin();
    memcpy(&fileHeader,bufferCursor,sizeof(sP7File_Header));
    bufferCursor+=sizeof(sP7File_Header);

    traceWindow->fileReadingStatus(0);
    while(bufferCursor<data.end())
    {
        QApplication::processEvents();
        memcpy(&chunkSize,bufferCursor,sizeof(tUINT32));
        chunkSize = GET_USER_HEADER_SIZE(chunkSize);
        std::cout<<chunkSize<<std::endl;
        if(chunkSize==0){
            break;
        }
        for(int i =0;i<chunkSize;i++)
        {
            dataVector.push_back(*bufferCursor);
            bufferCursor++;
        }
        chunkHandler.appendChunksQueue(dataVector);
        chunkHandler.waitCondition.wakeOne();
        dataVector.clear();
        chunkSize=0;
    }
    traceWindow->fileReadingStatus(100);
    file->close();
    delete file;
    data.clear();
    while(!chunkHandler.chunks.empty()){
        this->sleep(1);
    }
    chunkHandler.requestInterruption();
    chunkHandler.waitCondition.wakeOne();
    return true;
}

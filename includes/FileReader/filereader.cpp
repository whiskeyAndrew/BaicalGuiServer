//По каким-то неведомым мне причинам fread не мог читать нормально файл
//Вместо того чтобы читать файл оно читает какую-то хрень
//
#include "filereader.h"

//читаем и парсим .p7d

//файл в 2600kb отжирает 50мб оперативы
//надо будет посмотреть откуда такой жирный меморилик
FileReader::FileReader(QString fileName, TraceWindow* newTraceWindow)
{    
    file = new QFile(fileName);
    traceWindow = newTraceWindow;
    connect(this,FileReader::setSettingsDisabled,traceWindow,TraceWindow::setSettingsDisabled);
}

void FileReader::run()
{
    //неадекватное поведение если влезать в настройки до окончания чтения файла
    emit setSettingsDisabled(true);
    chunkHandler.setNeedBackup(false);
    chunkHandler.setTraceWindow(traceWindow);
    chunkHandler.start();
    //------------Открываем файл и читаем его в буфер----------//
    ReadFileData();
    //-------------Обработка----------------//
    HandlingChunks();
    emit setSettingsDisabled(false);
    //this->~FileReader();
    std::cout<<"------FileReader:: closing myself------"<<std::endl;
    traceWindow->setFileReader(NULL);
    if(isInterruptionRequested()){
        traceWindow->deleteLater();
    }
}

FileReader::~FileReader()
{

}

bool FileReader::ReadFileData()
{
    std::cout<<"Start reading file"<<std::endl;
    // Создаем объект класса QByteArray, куда мы будем считывать данные
    if (!file->open(QIODevice::ReadOnly)){
        return 0;
    }
    data = file->readAll();
    std::cout<<"File was read"<<std::endl;

}


bool FileReader::HandlingChunks()
{
    std::cout<<"start handling chunks"<<std::endl;
    sP7File_Header fileHeader;
    bufferCursor = data.begin();
    memcpy(&fileHeader,bufferCursor,sizeof(sP7File_Header));
    if(fileHeader.qwMarker!=P7_DAMP_FILE_MARKER_V1){
        return false;
    }
    bufferCursor+=sizeof(sP7File_Header);

    traceWindow->fileReadingStatus(0);
    while(bufferCursor<data.end() && !isInterruptionRequested())
    {
        //        QApplication::processEvents();
        memcpy(&chunkSize,bufferCursor,sizeof(tUINT32));
        chunkSize = GET_USER_HEADER_SIZE(chunkSize);
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

    file->close();
    delete file;
    data.clear();
    std::cout<<"ended handling chunks, waiting for chunkHandler to be empty"<<std::endl;
    while(!chunkHandler.chunks.empty() && !isInterruptionRequested()){
        //        QApplication::processEvents();
        this->sleep(1);
    }
    chunkHandler.requestInterruption();
    chunkHandler.waitCondition.wakeOne();
    traceWindow->fileReadingStatus(100);
    std::cout<<"ended all work"<<std::endl;
    return true;
}

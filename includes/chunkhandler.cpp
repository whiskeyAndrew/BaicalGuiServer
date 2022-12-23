#include "chunkhandler.h"


void ChunkHandler::run()
{

    //временный костыль для того, чтобы AutoOpen успевал первые пакеты прогрузить внутри окна.
    //Нужно удалить из кода когда запилю чтение из мапы при открытии окна
    Sleep(200);
    while((!chunks.empty()) || (fileEnded == false))
    {
        if(this->isInterruptionRequested()){
            std::cout<<"First interruption of chunkHandler"<<std::endl;
            break;
        }

        if(!getChunkFromQueue()){
            continue;
        }

        if(this->isInterruptionRequested()){
            std::cout<<"Second interruption of chunkHandler"<<std::endl;
            break;
        }
        processChunk();

        if(this->isInterruptionRequested()){
            std::cout<<"Thirs interruption of chunkHandler"<<std::endl;
            break;
        }
    }
    std::cout<<"------"<<"ChunkHandler is ending"<<"------"<<std::endl;

    if(needBackup){
        backupWriter.closeFile();
    }
    this->quit();
}

bool ChunkHandler::processChunk()
{
    //В метод мы передаем буфер с 4 байтами в самом начале, которые являются размером чанка. На всякий случай
    //Поэтому скипаем их
    chunkCursor+=sizeof(tUINT32);
    while(chunkCursor<chunkEnd)
    {
        memcpy(&ext_Raw,chunkCursor,sizeof(tUINT32));
        structType = GET_EXT_HEADER_TYPE(ext_Raw);
        structSubtype = GET_EXT_HEADER_SUBTYPE(ext_Raw);
        structSize = GET_EXT_HEADER_SIZE(ext_Raw);

        switch(structType)
        {
        case EP7USER_TYPE_TELEMETRY_V2:
        {
            //Делаем еще быстрее - просто скипаем чанк если знаем что у него внутри телеметрия, т.к. в чанке с телеметрией
            //не будет трейса
            //Получается я сделал ОПТИМИЗАЦИЮ?
            return 0;

            //весь код ниже - на случай если с телеметрией надо будет что-то делать, а не просто скипать ее
            switch(structSubtype)
            {
            //sP7Tel_Info
            case EP7TEL_TYPE_INFO:
            {
                chunkCursor+=structSize;
                //std::cout<<"V2 EP7TEL_TYPE_INFO skipped"<<std::endl;
                break;
            }

                //sP7Tel_Counter_v2+littleData
            case EP7TEL_TYPE_COUNTER:
            {
                chunkCursor+=structSize;
                //std::cout<<"V2 EP7TEL_TYPE_COUNTER skipped"<<std::endl;
                break;
            }

                //sP7Tel_Value_v2
            case EP7TEL_TYPE_VALUE:
            {
                chunkCursor+=structSize;
                //std::cout<<"V2 EP7TEL_TYPE_VALUE skipped"<<std::endl;
                break;
            }

                //sP7Tel_Enable_v2
            case EP7TEL_TYPE_ENABLE:
            {
                chunkCursor+=structSize;
                //std::cout<<"V2 EP7TEL_TYPE_ENABLE skipped"<<std::endl;
                break;
            }

                //ext_Raw and all?
            case EP7TEL_TYPE_CLOSE:
            {
                chunkCursor+=structSize; //вроде как оно просто ext_Raw прокидывает
                //std::cout<<"V2 EP7TEL_TYPE_CLOSE skipped"<<std::endl;
                break;
            }

                //ext_Raw and all?
            case EP7TEL_TYPE_DELETE:
            {
                chunkCursor+=structSize; //вроде как оно просто ExT_
                //std::cout<<"V2 EP7TEL_TYPE_DELETE skipped"<<std::endl;
                break;
            }

                //sP7Tel_Utc_Offs_V2
            case EP7TEL_TYPE_UTC_OFFS:
            {
                chunkCursor+=structSize;
                //std::cout<<"V2 EP7TEL_TYPE_UTC_OFFS skipped"<<std::endl;
                break;
            }
            default:
            {
                //std::cout<<"V2 Tele default enter, ext_Raw: "<<ext_Raw<<std::endl;
                break;
            }

            }
            break;
        }

        case EP7USER_TYPE_TRACE:
        {
            if(needBackup){
                //Сделано немного не так как в байкале
                //Байкал пишет чанки здоровенными кусками в файл и только потом их пишет
                //Я пишу размер каждого пришедшего куска + сам кусок, когда байкал копит куски и потом их размер пишет
                //Необязательно, но если бэкапы будут слишком жирными можно и пофиксить
                backupWriter.writeChunk(chunkCursor,structSize);
            }
            switch(structSubtype){
            case EP7TRACE_TYPE_DATA:
            {
                //Не уникальный трейс
                traceToGUI = trace.setTraceData(chunkCursor);
                if(isWindowOpened)
                {
                    emit sendTrace(traceToGUI);
                }
                chunkCursor = chunkCursor+structSize;
                break;
            }
            case EP7TRACE_TYPE_DESC:
            {
                //Уникальный трейс
                UniqueTraceData uTrace = trace.setTraceFormat(chunkCursor);

                //По какой-то причине инициализация соединений происходит позже, чем начинается обработка данных. Пока временный костыль
                if(!connectionEstablished){
                    connect(this,&ChunkHandler::sendUniqueTrace,traceWindow,&TraceWindow::addUniqueTrace);
                    connectionEstablished = true;
                }
                if(isWindowOpened)
                {
                    emit sendUniqueTrace(uTrace);
                }

                chunkCursor = chunkCursor+structSize;
                break;
            }
            case EP7TRACE_TYPE_INFO:
            {
                trace.setTraceInfo(chunkCursor);
                chunkCursor = chunkCursor+structSize;
                break;
            }
            case EP7TRACE_TYPE_THREAD_START:
            {
                trace.setTraceThreadStart(chunkCursor);
                chunkCursor+=structSize;
                break;
            }
            case EP7TRACE_TYPE_THREAD_STOP:
            {
                trace.setTraceThreadStop(chunkCursor);
                chunkCursor+=structSize;
                break;
            }
            case EP7TRACE_TYPE_MODULE:
            {
                trace.setTraceModule(chunkCursor);
                chunkCursor+=structSize;
                break;
            }
            case EP7TRACE_TYPE_UTC_OFFS:
            {
                trace.setTraceUTC(chunkCursor);
                chunkCursor+=structSize;
                break;
            }

            case EP7TRACE_TYPE_VERB:
            {
                //Пока не знаю что это
                break;
            }
            case EP7TRACE_TYPE_CLOSE:
            {
                //Закрытие соединения
                //Обычно происходит если приложение завершило работу
                //Наш сервак сам закроет соединение
                break;
            }
            default:
            {
                break;
            }
            }
            break;

        }

        case EP7USER_TYPE_TELEMETRY_V1:
        {

        }
        }

    }
    return 1;
}

ChunkHandler::ChunkHandler()
{

}


void ChunkHandler::setTraceWindow(TraceWindow* newTraceWindow)
{
    //Происходит позже чем обработка данных, исправить позже
    traceWindow = newTraceWindow;
    connect(this, &ChunkHandler::sendTrace,
            traceWindow, &TraceWindow::getTrace);
    connect(this,&ChunkHandler::sendTraceAsObject,traceWindow,&TraceWindow::setTraceAsObject);
    emit sendTraceAsObject(&trace);
    isWindowOpened = true;

}


void ChunkHandler::appendChunksQueue(std::vector<tINT8> newVector)
{
    mutex.tryLock(-1);
    chunks.push(newVector);
    mutex.unlock();
}

bool ChunkHandler::getChunkFromQueue()
{
    if(fileEnded==true && chunks.empty())
    {
        return true;
    }

    if(chunks.empty()){
        syncThreads.lock();
        waitCondition.wait(&syncThreads);
        syncThreads.unlock();
    }

    if(chunks.empty()){
        return false;
    }

    mutex.tryLock(-1);
    chunkVector = chunks.front();
    chunks.pop();
    mutex.unlock();
    chunkBuffer = chunkVector.data();
    chunkCursor = chunkBuffer;
    chunkEnd = chunkBuffer+chunkVector.size();
    return true;
}


void ChunkHandler::setNeedBackup(bool newNeedBackup)
{
    needBackup = newNeedBackup;
}

void ChunkHandler::setFileEnded(bool newFileEnded)
{
    this->fileEnded = fileEnded;
}

void ChunkHandler::initBackupWriter(tUINT32 dwProcess_ID, tUINT32 dwProcess_Start_Time_Hi, tUINT32 dwProcess_Start_Time_Lo)
{
    backupWriter.setFileHeader(dwProcess_ID,dwProcess_Start_Time_Hi,dwProcess_Start_Time_Lo);
}

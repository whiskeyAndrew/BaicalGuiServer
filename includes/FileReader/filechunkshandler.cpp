#include "filechunkshandler.h"
#include "filereader.h"

FileChunksHandler::FileChunksHandler()
{

}

void FileChunksHandler::SetTraceWindow(TraceWindow* newTraceWindow){
    traceWindow = newTraceWindow;
    connect(this,&FileChunksHandler::SendTraceFromFile,
            traceWindow,&TraceWindow::getTraceFromFile);
    //Для вывода данных по  трейсу через клик по строке нужно окно обращается к объекту Trace
    //Господи как же все не по ООПшному, надо когда-нибудь это все переделать, как же я говнокодил когда это делал
    connect(this,&FileChunksHandler::SendTraceAsObject,
            traceWindow,&TraceWindow::setTraceAsObject);
    emit(SendTraceAsObject(&trace));
}

void FileChunksHandler::AppendChunks(std::vector<tINT8> chunk)
{
    mutex.tryLock(-1);
    chunks.push(chunk);
    mutex.unlock();
}

void FileChunksHandler::setFileEnded(bool newFileEnded)
{
    fileEnded = newFileEnded;
}

void FileChunksHandler::run(){

    while(!chunks.empty() || fileEnded==false){
        std::cout<<chunks.size()<<std::endl;
        GetChunkFromQueue();
        ProcessChunk();
    }

    emit(SendTraceFromFile(traceQueue));

}

void FileChunksHandler::GetChunkFromQueue(){
    if(fileEnded==true && chunks.empty())
    {
        return;
    }

    while(chunks.empty())
    {
        continue;
    }

    mutex.tryLock(-1);
    chunkVector = chunks.front();
    chunks.pop();
    mutex.unlock();
    chunkBuffer = chunkVector.data();
    chunkCursor = chunkBuffer;
    chunkEnd = chunkBuffer+chunkVector.size();

}

bool FileChunksHandler::ProcessChunk()
{
    //В метод мы передаем буфер с 4 байтами в самом начале, которые являются размером чанка. На всякий случай
    //Поэтому скипаем их


    chunkCursor+=sizeof(tUINT32);
    while(chunkCursor<chunkEnd)
    {
        memcpy(&ext_Raw,chunkCursor,sizeof(tUINT32));
        structSubtype = GET_EXT_HEADER_SUBTYPE(ext_Raw);
        structSize = GET_EXT_HEADER_SIZE(ext_Raw);


        switch(structSubtype){
        case EP7TRACE_TYPE_DATA:
        {                //Не уникальный трейс
            TraceToGUI traceToGUI = trace.setTraceData(chunkCursor);
            traceQueue.push(traceToGUI);
            chunkCursor = chunkCursor+structSize;
            break;
        }
        case EP7TRACE_TYPE_DESC:
        {
            //Уникальный трейс
            trace.setTraceFormat(chunkCursor);
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
            //Пока непонятно что это
            break;
        }
        }
        break;

    }

    return 1;
}

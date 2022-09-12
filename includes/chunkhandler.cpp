#include "chunkhandler.h"

bool ChunkHandler::getWindowOpened() const
{
    return windowOpened;
}

void ChunkHandler::run()
{

    while(true)
    {
        GetChunkFromQueue();

        ProcessChunk();
    }
}

void ChunkHandler::setTraceWindow(TraceWindow *newTraceWindow)
{
    traceWindow = newTraceWindow;
    connect(this, &ChunkHandler::SendTrace,
            traceWindow, &TraceWindow::GetTrace);
    connect(this, &ChunkHandler::SendQueueSize,
            traceWindow, &TraceWindow::GetQueueSize);
    windowOpened = true;

}


bool ChunkHandler::AppendChunksQueue(std::vector<tINT8> newVector)
{
    mutex.tryLock(-1);
    chunks.push(newVector);
    mutex.unlock();
    //std::cout<<chunks.size()<<std::endl;
}

bool ChunkHandler::ProcessChunk()
{
    chunkCursor+=sizeof(tUINT32); //скипаем размер чанка
    while(chunkCursor<chunkEnd)
    {
        memcpy(&Ext_Raw,chunkCursor,sizeof(tUINT32));
        structType = GET_EXT_HEADER_TYPE(Ext_Raw);
        structSubtype = GET_EXT_HEADER_SUBTYPE(Ext_Raw);
        structSize = GET_EXT_HEADER_SIZE(Ext_Raw);

        switch(structType)
        {
        case EP7USER_TYPE_TELEMETRY_V2:
        {
            switch(structSubtype)
            {
            //sP7Tel_Info
            case EP7TEL_TYPE_INFO:
            {
                chunkCursor+=structSize;
                std::cout<<"V2 EP7TEL_TYPE_INFO skipped"<<std::endl;
                break;
            }

                //sP7Tel_Counter_v2+littleData
            case EP7TEL_TYPE_COUNTER:
            {
                chunkCursor+=structSize;
                std::cout<<"V2 EP7TEL_TYPE_COUNTER skipped"<<std::endl;
                break;
            }

                //sP7Tel_Value_v2
            case EP7TEL_TYPE_VALUE:
            {
                chunkCursor+=structSize;
                std::cout<<"V2 EP7TEL_TYPE_VALUE skipped"<<std::endl;
                break;
            }

                //sP7Tel_Enable_v2
            case EP7TEL_TYPE_ENABLE:
            {
                chunkCursor+=structSize;
                std::cout<<"V2 EP7TEL_TYPE_ENABLE skipped"<<std::endl;
                break;
            }

                //ext_raw and all?
            case EP7TEL_TYPE_CLOSE:
            {
                chunkCursor+=structSize; //вроде как оно просто Ext_Raw прокидывает
                std::cout<<"V2 EP7TEL_TYPE_CLOSE skipped"<<std::endl;
                break;
            }

                //ext_raw and all?
            case EP7TEL_TYPE_DELETE:
            {
                chunkCursor+=structSize; //вроде как оно просто ExT_
                std::cout<<"V2 EP7TEL_TYPE_DELETE skipped"<<std::endl;
                break;
            }

                //sP7Tel_Utc_Offs_V2
            case EP7TEL_TYPE_UTC_OFFS:
            {
                chunkCursor+=structSize;
                std::cout<<"V2 EP7TEL_TYPE_UTC_OFFS skipped"<<std::endl;
                break;
            }
            default:
            {
                std::cout<<"V2 Tele default enter, Ext_Raw: "<<Ext_Raw<<std::endl;
                break;
            }

            }
            break;
        }

        case EP7USER_TYPE_TRACE:
        {
            switch(structSubtype){
            case EP7TRACE_TYPE_DATA:
            {
                //Не уникальный трейс
                traceData = trace.setTraceData(chunkCursor);
                if(getWindowOpened())
                {
                    //traceWindow->GetTrace(traceData);
                    emit SendTrace(traceData);
                }
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

        case EP7USER_TYPE_TELEMETRY_V1:
        {

        }
        }

    }
}

ChunkHandler::ChunkHandler()
{

}

void ChunkHandler::GetChunkFromQueue()
{
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

    if(getWindowOpened())
    {
        emit SendQueueSize(chunks.size());
    }
}

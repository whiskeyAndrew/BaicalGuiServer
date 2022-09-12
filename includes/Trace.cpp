#include "Trace.h"
#include "tracewindow.h"

TraceLineData Trace::setTraceData(tINT8* chunkCursor)
{
    Args_ID *args;
    tINT64 argument = 0;
    TraceLineData traceDataPerLine;
    //Не уникальный трейс
    //Читаем его структуру и записываем в TraceLineData
    memcpy(&traceData,chunkCursor,sizeof(sP7Trace_Data));
    traceDataPerLine = uniqueTrace.value(traceData.wID);
    traceDataPerLine.dwSequence = traceData.dwSequence;
    chunkCursor+=sizeof(sP7Trace_Data);

    //Теперь надо прочитать аргументы и записать сюда
    //Кидаем поинтер на начало вектора структур, как только прочли нужное - перемещаем поинтер дальше
    traceDataPerLine.traceLineReadyToOutput = traceDataPerLine.traceLineData;
    if(traceDataPerLine.argsLen!=0)
    {
        for(int i =0; i<traceFormat.args_Len;i++)
        {
            memcpy(&argument,chunkCursor,traceDataPerLine.argsID[i].argSize);
            //Читаем аргументы, их размер и ID нам известен
            traceDataPerLine.argsValue.push_back(argument);
            chunkCursor+=traceDataPerLine.argsID[i].argSize;
        }

        traceDataPerLine = ReplaceArguments(traceDataPerLine);
    }
    return traceDataPerLine;
}

void Trace::setTraceFormat(tINT8* chunkCursor)
{
    Args_ID args;
    TraceLineData traceDataPerLine;
    //Уникальный трейс
    memcpy(&traceFormat,chunkCursor,sizeof(sP7Trace_Format));
    chunkCursor+=sizeof(sP7Trace_Format);
    traceDataPerLine.argsLen = traceFormat.args_Len;

    if(traceFormat.args_Len!=0)
    {
        for(int i =0; i<traceFormat.args_Len;i++)
        {
            //Заполняем вектор аргументов
            memcpy(&args,chunkCursor,sizeof(tUINT16));
            traceDataPerLine.argsID.push_back(args);
            chunkCursor+=sizeof(tUINT16);
        }
    }
    chunkCursor = ReadTraceText(chunkCursor, &traceDataPerLine);
    uniqueTrace.insert(traceFormat.wID,traceDataPerLine);
    traceToShow.insert(traceData.dwSequence,traceDataPerLine);
}

void Trace::setTraceUTC(tINT8* chunkCursor)
{
    memcpy(&traceUTC,chunkCursor,sizeof(sP7Trace_Utc_Offs));
}

void Trace::setTraceThreadStart(tINT8* chunkCursor)
{
    memcpy(&traceThreadStart,chunkCursor,sizeof(sP7Trace_Thread_Start));
}

void Trace::setTraceThreadStop(tINT8* chunkCursor)
{
    memcpy(&traceThreadStop,chunkCursor,sizeof(sP7Trace_Thread_Stop));
}
void Trace::setTraceModule(tINT8* chunkCursor)
{
    memcpy(&traceModule,chunkCursor,sizeof(sP7Trace_Module));
}


void Trace::setTraceInfo(tINT8* chunkCursor)
{
    memcpy(&traceInfo,chunkCursor,sizeof(sP7Trace_Info));
}

tINT8* Trace::ReadTraceText(tINT8* tempChunkCursor, TraceLineData *trace)
{
    while(*tempChunkCursor!='\0')
    {
        trace->traceLineData.push_back(tempChunkCursor);
        tempChunkCursor+=2;
    }
    tempChunkCursor+=2;

    while(*tempChunkCursor!='\0')
    {
        trace->fileDest.push_back(*tempChunkCursor);
        tempChunkCursor+=1;
    }

    tempChunkCursor+=1;

    while(*tempChunkCursor!='\0')
    {
        trace->functionName.push_back(*tempChunkCursor);
        tempChunkCursor+=1;
    }
    return tempChunkCursor;
}

//переделать
TraceLineData Trace::ReplaceArguments(TraceLineData trace)
{
    tUINT32 argNumber = 0;
    std::vector<QChar> typeSpecifier = {'d','b','i','o','u','x','X','s','c'};
    tUINT32 rememberPos = -1;


startFindingArgs:

    for(int lineIterator =0;lineIterator!=trace.traceLineReadyToOutput.length();lineIterator++)
    {
        if(argNumber==trace.argsLen)
            return trace;

        if(trace.traceLineReadyToOutput[lineIterator]=='%')
        {
            if(trace.traceLineReadyToOutput[lineIterator+1]=='%' || lineIterator==rememberPos)
            {
                rememberPos = lineIterator;
                lineIterator+=1;
                continue;
            }

            tUINT32 startPos = lineIterator;
            QString tempArgString;
            //Аргумент мы записываем в временный string
            for(tUINT32 argIterator = 0;argIterator<6;argIterator++){
                tempArgString.push_back(trace.traceLineReadyToOutput[lineIterator]);
                lineIterator++;
                //Находим конец аргумента по уникальным буквам typeSpecifier
                for(int it = 0; it<8;it++)
                {
                    if(tempArgString [argIterator] == typeSpecifier[it])
                    {
                        trace.traceLineReadyToOutput.replace(startPos,argIterator+1,QString::number(trace.argsValue[argNumber]));
                        argNumber++;
                        goto startFindingArgs;
                    }
                }
            }
        }
    }
}

//TraceLineData Trace::ReplaceArguments(TraceLineData trace)
//{
//    std::vector<QChar> argType = {'d','b','i','o','u','x','X','s','c'};
//    tINT32 argNumber = 0;
//    tINT32 replaceStart = -1;
//    trace.traceLineReadyToOutput = trace.traceLineData;

//    for(int i = 0; i<trace.traceLineReadyToOutput.length();i++)
//    {
//        if(argNumber==trace.argsLen)
//            break;

//        if(trace.traceLineReadyToOutput[i]=="%")
//        {
//            if(trace.traceLineReadyToOutput[i+1]=="%")
//            {
//                i++;
//                continue;
//            }
//            replaceStart = i;
//                i++;
//            for(int j = 0;j<6;j++)
//            {
//                if (std::find(argType.begin(),argType.end(), trace.traceLineReadyToOutput[i]) != argType.end())
//                {
//                    //Нашли конец аргумента
//                    trace.traceLineReadyToOutput.replace(replaceStart,i,QString::number(trace.argsValue[argNumber]));
//                    argNumber++;
//                    break;
//                }
//                else
//                {
//                    //Не нашли конец аргумента
//                    i++;
//                }
//            }
//            continue;
//        }
//    }
//    return trace;
//}

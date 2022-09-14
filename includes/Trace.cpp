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
    traceDataPerLine.traceData = traceData;
    chunkCursor+=sizeof(sP7Trace_Data);

    //Теперь надо прочитать аргументы и записать сюда
    //Кидаем поинтер на начало вектора структур, как только прочли нужное - перемещаем поинтер дальше
    traceDataPerLine.traceLineToGUI = traceDataPerLine.traceLineData;
    if(traceDataPerLine.traceFormat.args_Len!=0)
    {
        for(int i =0; i<traceDataPerLine.traceFormat.args_Len;i++)
        {
            memcpy(&argument,chunkCursor,traceDataPerLine.argsID[i].argSize);
            //Читаем аргументы, их размер и ID нам известен
            traceDataPerLine.argsValue.push_back(argument);
            chunkCursor+=traceDataPerLine.argsID[i].argSize;
        }

        traceDataPerLine = ReplaceArguments(traceDataPerLine);
    }
    traceToShow.insert(traceDataPerLine.traceData.dwSequence,traceDataPerLine);
    return traceDataPerLine;
}

void Trace::setTraceFormat(tINT8* chunkCursor)
{
    Args_ID args;
    TraceLineData traceDataPerLine;
    //Уникальный трейс
    memcpy(&traceFormat,chunkCursor,sizeof(sP7Trace_Format));
    chunkCursor+=sizeof(sP7Trace_Format);

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

    traceDataPerLine.traceFormat = traceFormat;
    uniqueTrace.insert(traceFormat.wID,traceDataPerLine);

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
    modules.insert(traceModule.wModuleId,traceModule);
}


void Trace::setTraceInfo(tINT8* chunkCursor)
{
    memcpy(&traceInfo,chunkCursor,sizeof(sP7Trace_Info));
}

QString Trace::getModule(tUINT32 moduleID)
{
    sP7Trace_Module module = modules.value(moduleID);
    return QString(module.pName);
}

TraceLineData Trace::GetTraceDataToGui(tUINT32 sequence)
{
    TraceLineData traceToGUI = traceToShow.value(sequence);
    return traceToGUI;
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

//катастрофической говнокод, надо переделать
TraceLineData Trace::ReplaceArguments(TraceLineData trace)
{
    tUINT32 argNumber = 0;
    std::vector<QChar> typeSpecifier = {'d','b','i','o','u','x','X','s','c','f'};
    tUINT32 rememberPos = -1;


startFindingArgs:

    for(int lineIterator =0;lineIterator<trace.traceLineToGUI.length();lineIterator++)
    {
        if(argNumber==trace.traceFormat.args_Len)
            return trace;

        if(trace.traceLineToGUI[lineIterator]=='%')
        {
            if(trace.traceLineToGUI[lineIterator+1]=='%' || lineIterator==rememberPos)
            {
                rememberPos = lineIterator;
                lineIterator+=1;
                continue;
            }

            tUINT32 startPos = lineIterator;
            QString tempArgString;
            //Аргумент мы записываем в временный string
            for(tUINT32 argIterator = 0;argIterator<6;argIterator++){
                tempArgString.push_back(trace.traceLineToGUI[lineIterator]);
                lineIterator++;
                //Находим конец аргумента по уникальным буквам typeSpecifier
                for(int it = 0; it<8;it++)
                {
                    if(tempArgString [argIterator] == typeSpecifier[it])
                    {
                        //Надо продумать как правильно все реализовать вокруг поинтеров
                        //Есть идея писать очередь указателей на ячейки памяти с элементами, чтобы мы их побайтово хранили
                        //И потом как-то доставать по айдишнику что ли
                        //Пока хз, надо обдумать
                        //Сейчас бы егэ по готовым решениям с стрингами в С
                        trace.traceLineToGUI.replace(startPos,argIterator+1,QString::number(trace.argsValue[argNumber]));
                        argNumber++;
                        goto startFindingArgs;
                    }
                }
            }
        }
    }
    return trace;
}

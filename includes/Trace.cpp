#include "Trace.h"
#include "tracewindow.h"
#include <sstream>

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
        std::string tempString;
        for(int i =0; i<traceDataPerLine.traceFormat.args_Len;i++)
        {
            memcpy(&argument,chunkCursor,traceDataPerLine.argsID[i].argSize);
            //Читаем аргументы, их размер и ID нам известен
            traceDataPerLine.argsValue.push_back(argument);
            chunkCursor+=traceDataPerLine.argsID[i].argSize;
        }

        traceDataPerLine.traceLineToGUI = formatVector(traceDataPerLine);
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

QString Trace::formatVector(TraceLineData trace)
{
    char argEnd[] = {'i','d','u','f'};
    int counter = 1;
    int index1;
    int index2;

    for(int i =0;i<trace.traceFormat.args_Len;i++)
    {
        bool found = false;
        index1 = trace.traceLineToGUI.indexOf('%');
        while(trace.traceLineToGUI[index1+1]=='%')
        {
            index1 = trace.traceLineToGUI.indexOf('%',index1+2);
        }

        for(int j =index1+1;j<index1+10;j++)
        {
            for(int k = 0;k<SIZE_OF_ARG_END;k++)
            {
                if(trace.traceLineToGUI[j]==argEnd[k])
                {
                    index2 = j+1;
                    trace.traceLineToGUI.replace(index1,index2-index1,QString::number(trace.argsValue[i]));
                    found = true;
                    break;
                }
            }

            if(found==true)
            {
                found = false;
                break;
            }
        }
    }
    return trace.traceLineToGUI;
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


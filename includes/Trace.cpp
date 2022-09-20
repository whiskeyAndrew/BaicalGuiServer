#include "Trace.h"
#include "tracewindow.h"
#include <sstream>

#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr

std::string string_format(const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

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

        for(int i=0;i<traceDataPerLine.traceFormat.args_Len;i++)
        {
            tempString = string_format(traceDataPerLine.traceLineData.toStdString(),traceDataPerLine.argsValue[i]);
        }
        traceDataPerLine.traceLineToGUI = QString::fromStdString(tempString);
        //traceDataPerLine = ReplaceArguments(traceDataPerLine);
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


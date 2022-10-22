#include "Trace.h"

TraceToGUI Trace::setTraceData(tINT8* chunkCursor)
{
    UniqueTraceData uniqueTrace;
    QString traceTextToGUI;
    //Не уникальный трейс
    //Читаем его структуру и записываем в TraceData
    memcpy(&traceData,chunkCursor,sizeof(sP7Trace_Data));
    chunkCursor+=sizeof(sP7Trace_Data);

    //Находим по wID структуру с остальными даннными трейса
    uniqueTrace = uniqueTraces.value(traceData.wID);

    //Теперь надо прочитать аргументы и записать в arguments, чтобы после этого записать в вектор
    //Кидаем поинтер на начало вектора структур, как только прочли нужное - перемещаем поинтер дальше
    if(uniqueTrace.traceFormat.args_Len!=0)
    {
        for(int i =0; i<uniqueTrace.traceFormat.args_Len;i++)
        {
            memcpy(&arguments,chunkCursor,uniqueTrace.argsID[i].argSize);
            //Читаем аргументы, их размер и ID нам известен
            argsValue.push_back(arguments);
            chunkCursor+=uniqueTrace.argsID[i].argSize;
        }
        traceTextToGUI = FormatVector(uniqueTrace.traceLineData,uniqueTrace.traceFormat.args_Len,argsValue);
    }
    else
    {
        traceTextToGUI = uniqueTrace.traceLineData;
    }

    argsValue.clear();
    traceToShow.insert(traceData.dwSequence,traceData);

    traceTime = CountTraceTime();

    return {traceTextToGUI,traceData.dwSequence,traceTime};
}

void Trace::setTraceFormat(tINT8* chunkCursor)
{
    UniqueTraceData uniqueTrace;
    //Уникальный трейс
    memcpy(&traceFormat,chunkCursor,sizeof(sP7Trace_Format));
    chunkCursor+=sizeof(sP7Trace_Format);

    if(traceFormat.args_Len!=0)
    {
        for(int i =0; i<traceFormat.args_Len;i++)
        {
            //Заполняем вектор аргументов
            memcpy(&argumentsData,chunkCursor,sizeof(tUINT16));
            uniqueTrace.argsID.push_back(argumentsData);
            chunkCursor+=sizeof(tUINT16);
        }
    }

    ReadTraceText(chunkCursor, &uniqueTrace);

    uniqueTrace.traceFormat = traceFormat;
    needToShow.insert(traceFormat.wID,true);
    uniqueTraces.insert(traceFormat.wID,uniqueTrace);

}

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 )
    {
        throw std::runtime_error( "Error during formatting." );
    }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

//Переписать когда пройдет тильт
QString Trace::FormatVector(QString str, int argsCount, std::vector<tUINT64> args)
{
    char argEnd[] = {'i','d','u','f','X'};
    int index1;
    int index2;
    QString tempString;
    std::string tempStringSTD;
    bool found = false;
    std::string toOutput;

    for(int i =0;i<argsCount;i++){
        int index1 = str.indexOf('%');

        for(int j =index1+1;j<index1+10;j++)
        {
            for(int k = 0;k<SIZE_OF_ARG_END;k++)
            {
                if(str[j]==argEnd[k])
                {
                    index2 = j+1;
                    tempString = str.left(index2);
                    str.remove(0,index2);
                    tempStringSTD = tempString.toStdString();
                    tempStringSTD = string_format(tempStringSTD,args[i]);
                    toOutput+=tempStringSTD;
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
    toOutput+=str.toStdString();
    return QString::fromStdString(toOutput);
}

SYSTEMTIME Trace::CountTraceTime(){
    FILETIME creationTime;
    SYSTEMTIME toSystemTime;
    tUINT32 tempTime = (traceData.qwTimer-traceInfo.qwTimer_Value)/traceInfo.qwTimer_Frequency;

    creationTime.dwLowDateTime = traceInfo.dwTime_Lo+tempTime;
    creationTime.dwHighDateTime = traceInfo.dwTime_Hi;

    FileTimeToSystemTime(&creationTime,&toSystemTime);
    SystemTimeToTzSpecificLocalTime(NULL,&toSystemTime,&toSystemTime);

   return toSystemTime;
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
    return QString(modules.value(moduleID).pName);
}

sP7Trace_Data Trace::GetTraceData(tUINT32 sequence)
{
    return traceToShow.value(sequence);
}

UniqueTraceData Trace::GetTraceFormat(tUINT32 wID)
{
    return uniqueTraces.value(wID);
}

tINT8* Trace::ReadTraceText(tINT8* tempChunkCursor, UniqueTraceData *trace)
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


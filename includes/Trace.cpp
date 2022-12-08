#include "Trace.h"
#include "qdatetime.h"
#include <ctime>
#include "time.h"
static __attribute__ ((unused)) void UnpackLocalTime(tUINT64  i_qwTime,
                                                     tUINT32 &o_rYear,
                                                     tUINT32 &o_rMonth,
                                                     tUINT32 &o_rDay,
                                                     tUINT32 &o_rHour,
                                                     tUINT32 &o_rMinutes,
                                                     tUINT32 &o_rSeconds,
                                                     tUINT32 &o_rMilliseconds,
                                                     tUINT32 &o_rMicroseconds,
                                                     tUINT32 &o_rNanoseconds
                                                     )
{
    tUINT32 l_dwReminder = i_qwTime % TIME_MLSC_100NS; //micro & 100xNanoseconds
    tUINT32 l_dwNano     = i_qwTime % 10;
    tUINT32 l_dwMicro    = l_dwReminder / 10;

    i_qwTime -= l_dwReminder;

    tUINT32 l_dwMilli = (i_qwTime % TIME_SEC_100NS) / TIME_MLSC_100NS;

    i_qwTime -= TIME_OFFSET_1601_1970;

    time_t  l_llTime = i_qwTime / TIME_SEC_100NS;
    tm     *l_pTime  = localtime(&l_llTime);
    if (l_pTime)
    {
        o_rYear         = 1900 + l_pTime->tm_year;
        o_rMonth        = 1 + l_pTime->tm_mon;
        o_rDay          = l_pTime->tm_mday;
        o_rHour         = l_pTime->tm_hour;
        o_rMinutes      = l_pTime->tm_min;
        o_rSeconds      = l_pTime->tm_sec;
        o_rMilliseconds = l_dwMilli;
        o_rMicroseconds = l_dwMicro;
        o_rNanoseconds  = l_dwNano;
    }
    else
    {
        o_rYear         = 0;
        o_rMonth        = 0;
        o_rDay          = 0;
        o_rHour         = 0;
        o_rMinutes      = 0;
        o_rSeconds      = 0;
        o_rMilliseconds = l_dwMilli;
        o_rMicroseconds = l_dwMicro;
        o_rNanoseconds  = l_dwNano;
    }
}//UnpackLocalTime

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
        traceTextToGUI = FormatVector(uniqueTrace.traceLineData,uniqueTrace.traceFormat.args_Len,argsValue,uniqueTrace.traceFormat.wID);
    }
    else
    {
        traceTextToGUI = uniqueTrace.traceLineData;
    }

    argsValue.clear();
    traceToShow.insert(traceData.dwSequence,traceData);

    //traceTime = CountTraceTime();

    return {traceTextToGUI,traceData.dwSequence,CountTraceTime(),uniqueTrace.traceFormat.wID,traceData.bLevel};
}

UniqueTraceData Trace::setTraceFormat(tINT8* chunkCursor)
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

    //перепроверить че это за строка такая
    uniqueTrace.traceFormat = traceFormat;

    uniqueTraces.insert(traceFormat.wID,uniqueTrace);
    return uniqueTrace;
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
QString Trace::FormatVector(QString str, int argsCount, std::vector<tUINT64> args,tUINT32 wID)
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
                    if(tracesThatNeedEnumChange.contains(wID)){
//                        tUINT32 lastArg = tempStringSTD.find_last_of(args[i]);
                        tempStringSTD.erase(index1,tempStringSTD.size());
                        tempStringSTD.append(enums->at(tracesThatNeedEnumChange.value(wID)).enums.at(i).name.toStdString());
                    }
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

p7Time Trace::CountTraceTime(){
    tUINT64 l_dbTimeDiff = (((tDOUBLE)traceData.qwTimer - (tDOUBLE)traceInfo.qwTimer_Value) * (tDOUBLE)TIME_SEC_100NS) / (tDOUBLE)traceInfo.qwTimer_Frequency;
    tUINT64 m_qwStreamTime = (tUINT64)traceInfo.dwTime_Lo + (((tUINT64)traceInfo.dwTime_Hi) << 32);

    p7Time time;
    UnpackLocalTime(m_qwStreamTime+l_dbTimeDiff,
                    time.dwYear,
                    time.dwMonth,
                    time.dwDay,
                    time.dwHour,
                    time.dwMinutes,
                    time.dwSeconds,
                    time.dwMilliseconds,
                    time.dwMicroseconds,
                    time.dwNanoseconds);
    FILETIME creationTime;
    SYSTEMTIME toSystemTime;
    tUINT32 tempTime = (traceData.qwTimer-traceInfo.qwTimer_Value);/*traceInfo.qwTimer_Frequency*1000;*/

    creationTime.dwLowDateTime = traceInfo.dwTime_Lo+tempTime;
    creationTime.dwHighDateTime = traceInfo.dwTime_Hi;

    FileTimeToSystemTime(&creationTime,&toSystemTime);
    SystemTimeToTzSpecificLocalTime(NULL,&toSystemTime,&toSystemTime);

    return time;
}

void Trace::Test()
{
    for(UniqueTraceData uq:uniqueTraces.values()){
        std::cout<<uq.traceLineData.toStdString()<<std::endl;
    }
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

void Trace::SetEnumsList(QList<likeEnum> *newEnums)
{
    enums = newEnums;
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

void Trace::AppendTraceThatNeedEnumInsteadOfArgs(tUINT32 wID, tUINT32 enumId){
    tracesThatNeedEnumChange.insert(wID,enumId);
    std::cout<<"Appended"<<std::endl;
}

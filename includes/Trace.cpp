#include "Trace.h"


//Обработчик трейсов. Обрабатывает пришедшие ему маленькие чанки, внутри которых содержатся различные данные о трейсах. memcpy - сила



GUIData Trace::setTraceData(tINT8* chunkCursor)
{
    UniqueTraceData uniqueTrace;
    QString traceTextToGUI;

    //Не уникальный трейс
    //Читаем его структуру и записываем в TraceData
    memcpy(&traceData,chunkCursor,sizeof(sP7Trace_Data));
    chunkCursor+=sizeof(sP7Trace_Data);

    //Находим по wID структуру с остальными даннными трейса
    uniqueTrace = uniqueTraces.value(traceData.wID);

    if(uniqueTrace.traceFormat.args_Len!=0){
        for(int i =0; i<uniqueTrace.traceFormat.args_Len;i++){
            memcpy(&arguments,chunkCursor,uniqueTrace.argsID[i].argSize);
            //Читаем аргументы, их размер и ID нам известен
            argsValue.push_back(arguments);
            chunkCursor+=uniqueTrace.argsID[i].argSize;
        }
    }

    //traceTime = CountTraceTime();
    GUIData traceToGUI = {traceData,0,1,argsValue};
    argsValue.clear();
    return traceToGUI;
}

UniqueTraceData Trace::setTraceFormat(tINT8* chunkCursor)
{
    UniqueTraceData uniqueTrace;
    //Уникальный трейс
    memcpy(&traceFormat,chunkCursor,sizeof(sP7Trace_Format));
    chunkCursor+=sizeof(sP7Trace_Format);

    if(traceFormat.args_Len!=0){
        for(int i =0; i<traceFormat.args_Len;i++){
            //Заполняем вектор аргументов
            memcpy(&argumentsData,chunkCursor,sizeof(tUINT16));
            uniqueTrace.argsID.push_back(argumentsData);
            chunkCursor+=sizeof(tUINT16);
        }
    }

    readTraceText(chunkCursor, &uniqueTrace);

    //перепроверить че это за строка такая
    uniqueTrace.traceFormat = traceFormat;

    if(traceFormat.args_Len!=0){
        std::vector<tUINT64> tempVector;
        for(tUINT64 i =1; i<=uniqueTrace.traceFormat.args_Len;i++){
            tempVector.push_back(i);
        }
        uniqueTrace.traceLineForEnumWindow = formatVector(&uniqueTrace,tempVector,NULL);
    } else{
        uniqueTrace.traceLineForEnumWindow = uniqueTrace.traceLineData;
    }
    uniqueTrace.moduleId = traceFormat.moduleID;
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
    //    std::unique_ptr<char[]> buf( new char[ size ] );
    char* buf = new char[size];
    std::snprintf( buf, size, format.c_str(), args ... );
    std::string returnable = std::string( buf, buf + size - 1 );
    delete[] buf;
    return returnable; // We don't want the '\0' inside
}

//Переписать когда пройдет тильт
QString Trace::formatVector(UniqueTraceData* uniqueTrace, std::vector<tUINT64> args, QList<ArgsPosition>* argsPosition)
{

    QString str = uniqueTrace->traceLineData;
    tUINT32 argsCount = uniqueTrace->traceFormat.args_Len;
    tUINT32 wID = uniqueTrace->traceFormat.wID;
    char argEnd[] = {'i','d','u','f','X'};
    int index1;
    int index2;
    QString tempString;
    std::string tempStringSTD;
    bool found = false;
    QString toOutput;

    for(int i =0;i<argsCount;i++){
        int index1 = str.indexOf('%');

        for(int j =index1+1;j<index1+10;j++){
            for(int k = 0;k<SIZE_OF_ARG_END;k++){
                if(str[j]==argEnd[k]){
                    index2 = j+1;
                    tempString = str.left(index2);
                    str.remove(0,index2);
                    tempStringSTD = tempString.toStdString();
                    tempStringSTD = string_format(tempStringSTD,args[i]);
                    tUINT32 toOutputSizeBeforeUpdate = toOutput.size();
                    toOutput.append(QString::fromStdString(tempStringSTD));
                    if(argsPosition==NULL){
                        toOutput.replace(toOutputSizeBeforeUpdate+index1,toOutput.length()-toOutputSizeBeforeUpdate+index1,"{"+QString::number(args[i])+"}");
                    }
                    else{
                        argsPosition->append({toOutputSizeBeforeUpdate+index1,tUINT32(toOutput.length())});
                    }
                    found = true;
                    break;
                }
            }

            if(found==true){
                found = false;
                break;
            }
        }
    }
    toOutput+=str;
    return toOutput;
}

p7Time Trace::countTraceTime(sP7Trace_Data data){
    tDOUBLE l_dbTimeDiff = (((tDOUBLE)data.qwTimer - (tDOUBLE)traceInfo.qwTimer_Value)*  (tDOUBLE)TIME_SEC_100NS) / (tDOUBLE)traceInfo.qwTimer_Frequency;
    tUINT64 m_qwStreamTime = (tUINT64)traceInfo.dwTime_Lo + (((tUINT64)traceInfo.dwTime_Hi) << 32);

    p7Time time;
    UnpackLocalTime(m_qwStreamTime+(tUINT64)l_dbTimeDiff+(tUINT64)traceUTC.iUTCOffsetSec,
                    time.dwYear,
                    time.dwMonth,
                    time.dwDay,
                    time.dwHour,
                    time.dwMinutes,
                    time.dwSeconds,
                    time.dwMilliseconds);
    return time;
}

const QMap<tUINT32, UniqueTraceData> &Trace::getUniqueTraces() const
{
    return uniqueTraces;
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

sP7Trace_Module Trace::setTraceModule(tINT8* chunkCursor)
{
    memcpy(&traceModule,chunkCursor,sizeof(sP7Trace_Module));
    modules.insert(traceModule.wModuleId,traceModule);
    return traceModule;
}


void Trace::setTraceInfo(tINT8* chunkCursor)
{
    memcpy(&traceInfo,chunkCursor,sizeof(sP7Trace_Info));
}

QString Trace::getModule(tUINT32 moduleID)
{
    return QString(modules.value(moduleID).pName);
}

tINT8* Trace::readTraceText(tINT8* tempChunkCursor, UniqueTraceData* trace)
{
    while(*tempChunkCursor!='\0'){
        trace->traceLineData.push_back(tempChunkCursor);
        tempChunkCursor+=2;
    }
    tempChunkCursor+=2;

    while(*tempChunkCursor!='\0'){
        trace->fileDest.push_back(*tempChunkCursor);
        tempChunkCursor+=1;
    }

    tempChunkCursor+=1;

    while(*tempChunkCursor!='\0'){
        trace->functionName.push_back(*tempChunkCursor);
        tempChunkCursor+=1;
    }
    return tempChunkCursor;
}

void Trace::UnpackLocalTime(tUINT64  i_qwTime,
                     short &o_rYear,
                     short &o_rMonth,
                     short &o_rDay,
                     short &o_rHour,
                     short &o_rMinutes,
                     short &o_rSeconds,
                     short &o_rMilliseconds
                     )
{
    tUINT32 l_dwReminder = i_qwTime % TIME_MLSC_100NS; //micro & 100xNanoseconds
    //    tUINT32 l_dwNano     = i_qwTime % 10;
    //    tUINT32 l_dwMicro    = l_dwReminder / 10;

    i_qwTime -= l_dwReminder;

    tUINT32 l_dwMilli = (i_qwTime % TIME_SEC_100NS) / TIME_MLSC_100NS;

    i_qwTime -= TIME_OFFSET_1601_1970;

    time_t  l_llTime = i_qwTime / TIME_SEC_100NS;
    tm    * l_pTime  = localtime(&l_llTime);
    if (l_pTime){
        o_rYear         = 1900 + l_pTime->tm_year;
        o_rMonth        = 1 + l_pTime->tm_mon;
        o_rDay          = l_pTime->tm_mday;
        o_rHour         = l_pTime->tm_hour;
        o_rMinutes      = l_pTime->tm_min;
        o_rSeconds      = l_pTime->tm_sec;
        o_rMilliseconds = l_dwMilli;
        //        o_rMicroseconds = l_dwMicro;
        //        o_rNanoseconds  = l_dwNano;
    }
    else{
        o_rYear         = 0;
        o_rMonth        = 0;
        o_rDay          = 0;
        o_rHour         = 0;
        o_rMinutes      = 0;
        o_rSeconds      = 0;
        o_rMilliseconds = l_dwMilli;
        //        o_rMicroseconds = l_dwMicro;
        //        o_rNanoseconds  = l_dwNano;
    }
}//UnpackLocalTime

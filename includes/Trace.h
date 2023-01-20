#ifndef TRACE_H
#define TRACE_H
#include "GTypes.h"
#include <iostream>
#include <vector>
#include <QString>
#include <QMap>
#include "enumparser.h"
#include <memory>
#include "debuglogger.h"

#define TIME_OFFSET_1601_1970                            (116444736000000000ULL)

#define TIME_HRS_100NS                                            36000000000ull
#define TIME_MIN_100NS                                              600000000ull
#define TIME_SEC_100NS                                               10000000ull
#define TIME_MLSC_100NS                                                 10000ull
#define TIME_MCSC_100NS                                                    10ull

#define TIME_ZONE_AVALIABLE


#define SIZE_OF_ARG_END 5

//Для применения енама мы запоминаем где хранятся аргументы внутри обработанной строки и будет заменять их на енамы
struct ArgsPosition{
    tUINT32 argStart;
    tUINT32 argEnd;
};



struct p7Time{
    tUINT32        dwYear;            //year
    tUINT32        dwMonth;           //month
    tUINT32        dwDay;             //day
    tUINT32        dwHour;            //hour
    tUINT32        dwMinutes;         //minute
    tUINT32        dwSeconds;         //seconds
    tUINT32        dwMilliseconds;    //milliseconds
    tUINT32        dwMicroseconds;    //microseconds
    tUINT32        dwNanoseconds;     //nanoseconds
};

struct GUIData{
    tUINT32 sequence;
    QString trace;
    tUINT32 wID;
    tUINT32 bLevel;
    QList<ArgsPosition> argsPosition;
    p7Time time;
    tINT32 positionInMap;
};
class TraceWindow;
#pragma pack(push,2)
struct sP7Trace_Info
{
    tUINT32    sCommonRaw;
    tUINT32       dwTime_Hi; //4 байта, время создания Trace
    tUINT32       dwTime_Lo; //4  байта, время создания Trace
    tUINT64       qwTimer_Value; //8 байт, значение таймера (?)
    tUINT64       qwTimer_Frequency; //8 байт, Частота таймера (?)
    tUINT64       qwFlags; //8 байт, пока непонятно
    char pName[128]; //128 байт, название Trace
};

struct Args_ID
{
    tUINT8 argID;
    tUINT8 argSize;
};

struct sP7Trace_Utc_Offs
{
    tUINT32 SP7ext_Raw;
    tINT32 iUTCOffsetSec;
};

struct sP7Trace_Thread_Start
{
    tUINT32 SP7ext_Raw;
    tUINT32 dwThreadID;
    tUINT64 qwTimer;
    char pName[48];
};

struct sP7Trace_Module
{
    tUINT32 SP7ext_Raw;
    tUINT16 wModuleId;
    tUINT32 traceLevel;
    char pName[54];
};

struct sP7Trace_Thread_Stop
{
    tUINT32 sP7ext_Raw;
    tUINT32 dwThreadID;
    tUINT64 qwTimer;
};

struct sP7Trace_Format
{
    tUINT32 sP7ext_Raw;
    tUINT16 wID;
    tUINT16 line;
    tUINT16 moduleID;
    tUINT16 args_Len;
};

struct sP7Trace_Data
{
    tUINT32 sP7ext_Raw;
    tUINT16 wID;
    tUINT8 bLevel;
    tUINT8 bProcessor;
    tUINT32 dwThreadID;
    tUINT32 dwSequence;
    tUINT64 qwTimer;
};



struct TraceToGUI
{
    QString trace;
    tUINT32 sequence;
    p7Time traceTime;
    tUINT32 wID;
    tUINT32 bLevel;
    QList<ArgsPosition> argsPositionAfterFormatting;
};


struct UniqueTraceData
{
    sP7Trace_Format traceFormat;
    QString traceLineData;
    QString fileDest;
    QString functionName;
    std::vector<Args_ID> argsID;
    QString traceLineForEnumWindow;
    tUINT32 moduleId;
};


#pragma pack(pop)

class Trace
{
private:
    sP7Trace_Info traceInfo;
    sP7Trace_Utc_Offs traceUTC;
    sP7Trace_Thread_Start traceThreadStart;
    sP7Trace_Module traceModule;
    sP7Trace_Thread_Stop traceThreadStop;
    sP7Trace_Format traceFormat;
    sP7Trace_Data traceData;

    std::vector<tUINT64> argsValue;

    QMap<tUINT32, bool> needToShow;
    QMap<tUINT32,sP7Trace_Data> traceToShow;
    QMap<tUINT32,sP7Trace_Module> modules;
    tINT64 arguments = 0;
    Args_ID argumentsData;

    SYSTEMTIME traceTime;

    QString formatVector(UniqueTraceData* uniqueTrace, std::vector<tUINT64> args, QList<ArgsPosition>* argsPosition);
    tINT8* readTraceText(tINT8* chunkCursor, UniqueTraceData* trace);
    p7Time countTraceTime();
public:
    //TraceLineData traceDataPerLine;
    QMap<tUINT32,UniqueTraceData> uniqueTraces;
    sP7Trace_Data getTraceData(tUINT32 sequence);
    UniqueTraceData getTraceFormat(tUINT32 wID);

    void setTraceInfo(tINT8* chunkPointer);
    void setTraceUTC(tINT8* chunkCursor);
    void setTraceThreadStart(tINT8* chunkCursor);
    sP7Trace_Module setTraceModule(tINT8* chunkCursor);
    void setTraceThreadStop(tINT8* chunkCursor);
    UniqueTraceData setTraceFormat(tINT8* chunkCursor);

    TraceToGUI setTraceData(tINT8* chunkCursor);

    QString getModule(tUINT32 moduleID);
};
#endif // TRACE_H

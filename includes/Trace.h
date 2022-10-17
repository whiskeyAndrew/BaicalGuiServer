#ifndef TRACE_H
#define TRACE_H
#include "GTypes.h"
#include <iostream>
#include <vector>
#include <QString>
#include <QMap>

#include <memory>


#define SIZE_OF_ARG_END 5

class TraceWindow;
#pragma pack(push,2)
struct sP7Trace_Info
{
    tINT32    sCommonRaw;
    tINT32       dwTime_Hi; //4 байта, время создания Trace
    tINT32       dwTime_Lo; //4  байта, время создания Trace
    tUINT64       qwTimer_Value; //8 байт, значение таймера (?)
    tUINT64       qwTimer_Frequency; //8 байт, Частота таймера (?)
    tUINT64       qwFlags; //8 байт, пока непонятно
    char pName[128]; //128 байт, название Trace
};

struct Args_ID
{
    tINT8 argID;
    tINT8 argSize;
};

struct sP7Trace_Utc_Offs
{
    tINT32 SP7Ext_Raw;
    tINT32 iUTCOffsetSec;
};

struct sP7Trace_Thread_Start
{
    tINT32 SP7Ext_Raw;
    tINT32 dwThreadID;
    tUINT64 qwTimer;
    char pName[48];
};

struct sP7Trace_Module
{
    tINT32 SP7Ext_Raw;
    tUINT16 wModuleId;
    tINT32 traceLevel;
    char pName[54];
};

struct sP7Trace_Thread_Stop
{
    tINT32 sP7Ext_Raw;
    tINT32 dwThreadID;
    tUINT64 qwTimer;
};

struct sP7Trace_Format
{
    tINT32 sP7Ext_Raw;
    tUINT16 wID;
    tUINT16 line;
    tUINT16 moduleID;
    tUINT16 args_Len;
};

struct sP7Trace_Data
{
    tUINT32 sP7Ext_Raw;
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
    SYSTEMTIME traceTime;
};

struct UniqueTraceData
{
    sP7Trace_Format traceFormat;
    QString traceLineData;
    QString fileDest;
    QString functionName;
    std::vector<Args_ID> argsID;
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
    QMap<tUINT32,UniqueTraceData> uniqueTraces;
    QMap<tUINT32,sP7Trace_Data> traceToShow;
    QMap<tUINT32,sP7Trace_Module> modules;

    tINT64 arguments = 0;
    Args_ID argumentsData;

    SYSTEMTIME traceTime;

    QString FormatVector(QString str, int argsCount, std::vector<tUINT64> args);
    tINT8* ReadTraceText(tINT8* chunkCursor, UniqueTraceData *trace);
    SYSTEMTIME CountTraceTime();
public:
    //TraceLineData traceDataPerLine;
    sP7Trace_Data GetTraceData(tUINT32 sequence);
    UniqueTraceData GetTraceFormat(tUINT32 wID);

    void setTraceInfo(tINT8* chunkPointer);
    void setTraceUTC(tINT8* chunkCursor);
    void setTraceThreadStart(tINT8* chunkCursor);
    void setTraceModule(tINT8* chunkCursor);
    void setTraceThreadStop(tINT8* chunkCursor);
    void setTraceFormat(tINT8* chunkCursor);

    TraceToGUI setTraceData(tINT8* chunkCursor);

    QString getModule(tUINT32 moduleID);
};
#endif // TRACE_H

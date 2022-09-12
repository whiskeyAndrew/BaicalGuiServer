#ifndef TRACE_H
#define TRACE_H
#include "GTypes.h"
#include <iostream>
#include <vector>
#include <QString>
#include <QMap>

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
    tINT32 sP7Ext_Raw;
    tUINT16 wID;
    tUINT8 bLevel;
    tUINT8 bProcessor;
    tINT32 dwThreadID;
    tINT32 dwSequence;
    tUINT64 qwTimer;
};

struct TraceLineData
{
    QString traceLineData;
    QString traceLineReadyToOutput;
    QString fileDest;
    QString functionName;
    tUINT32 dwSequence;
    tINT32 argsLen;
    std::vector<Args_ID> argsID;
    std::vector<tUINT64> argsValue;
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

    QMap<tUINT32,TraceLineData> uniqueTrace;
    QMap<tUINT32,TraceLineData> traceToShow;
public:
    //TraceLineData traceDataPerLine;
    void setTraceInfo(tINT8* chunkPointer);
    void setTraceUTC(tINT8* chunkCursor);
    void setTraceThreadStart(tINT8* chunkCursor);
    void setTraceModule(tINT8* chunkCursor);
    void setTraceThreadStop(tINT8* chunkCursor);
    void setTraceFormat(tINT8* chunkCursor);
    TraceLineData ReplaceArguments(TraceLineData trace);
    TraceLineData setTraceData(tINT8* chunkCursor);
    tINT8* ReadTraceText(tINT8* chunkCursor, TraceLineData *trace);

};
#endif // TRACE_H

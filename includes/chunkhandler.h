#ifndef CHUNKHANDLER_H
#define CHUNKHANDLER_H

#include <QThread>
#include <queue>
#include "GTypes.h"
#include <QMutex>
#include "Trace.h"
#include "tracewindow.h"
#define P7_EXTENSION_TYPE_BITS_COUNT                                         (5)
#define P7_EXTENSION_SUB_TYPE_BITS_COUNT                                     (5)
#define P7_EXTENSION_PACKET_SIZE_BITS_COUNT                                 (22)
#define P7TELEMETRY_NAME_LENGTH                                             (64)
#define P7TELEMETRY_COUNTER_NAME_LENGTH_V1                                  (64)

#define P7_EXTENSION_MAX_TYPES               (1 << P7_EXTENSION_TYPE_BITS_COUNT)

#define GET_EXT_HEADER_SIZE(iHeaderRaw)\
    (iHeaderRaw >> (P7_EXTENSION_TYPE_BITS_COUNT + P7_EXTENSION_SUB_TYPE_BITS_COUNT))

#define GET_EXT_HEADER_TYPE(iHeaderRaw)\
    (iHeaderRaw & ((1 << P7_EXTENSION_TYPE_BITS_COUNT) - 1))

#define GET_EXT_HEADER_SUBTYPE(iHeaderRaw)\
    ((iHeaderRaw >> P7_EXTENSION_TYPE_BITS_COUNT) & ((1 << P7_EXTENSION_SUB_TYPE_BITS_COUNT) - 1))

enum eP7User_Type
{
    EP7USER_TYPE_TRACE          =  0,
    EP7USER_TYPE_TELEMETRY_V1       ,
    EP7USER_TYPE_TELEMETRY_V2       ,

    EP7USER_TYPE_MAX            = P7_EXTENSION_MAX_TYPES
};

enum eP7Trace_Type
{
    EP7TRACE_TYPE_INFO          =  0, //Client->Server
    EP7TRACE_TYPE_DESC              , //Client->Server
    EP7TRACE_TYPE_DATA              , //Client->Server
    EP7TRACE_TYPE_VERB              , //Client->Server->Client
    EP7TRACE_TYPE_CLOSE             , //Client->Server
    EP7TRACE_TYPE_THREAD_START      , //Client->Server
    EP7TRACE_TYPE_THREAD_STOP       , //Client->Server
    EP7TRACE_TYPE_MODULE            , //Client->Server
    EP7TRACE_TYPE_DELETE            , //Server->Client
    EP7TRACE_TYPE_UTC_OFFS          , //Client->Server

    EP7TRACE_TYPE_MAX           = 32
};

enum eP7Tel_Type
{
    EP7TEL_TYPE_INFO            =  0, //Client->Server
    EP7TEL_TYPE_COUNTER             , //Client->Server
    EP7TEL_TYPE_VALUE               , //Client->Server
    EP7TEL_TYPE_ENABLE              , //Server->Client
    EP7TEL_TYPE_CLOSE               , //Client->Server
    EP7TEL_TYPE_DELETE              , //Server->Client
    EP7TEL_TYPE_UTC_OFFS            , //Client->Server

    EP7TEL_TYPE_MAX             = 32
};
class ChunkHandler : public QThread
{
    Q_OBJECT
private:
    Trace trace;
    bool windowOpened = false;
    TraceWindow *traceWindow;
    TraceLineData traceData;
    std::queue<std::vector<tINT8>> chunks;
    QMutex mutex;
    void run();
    std::vector<tINT8> chunkVector;

    tINT8* chunkBuffer;
    tINT8* chunkCursor;
    tINT8* chunkEnd;
    tINT8* objectEnd; //нужен для определения конца читаемой структурки

    tUINT32 Ext_Raw;
    tUINT32 structType;
    tUINT32 structSubtype;
    tUINT32 structSize;


public:

    bool AppendChunksQueue(std::vector<tINT8> newVector);
    bool ProcessChunk();
    ChunkHandler();
    void GetChunkFromQueue();

    void setTraceWindow(TraceWindow *newTraceWindow);
    bool getWindowOpened() const;

signals:
    void SendTrace(TraceLineData trace);
    void SendQueueSize(tUINT32 size);
};

#endif // CHUNKHANDLER_H

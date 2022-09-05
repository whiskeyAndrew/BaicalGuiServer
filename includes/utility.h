#ifndef UTILITY_H
#define UTILITY_H
#endif // UTILITY_H

#include "includes.h"

#define USER_PACKET_CHANNEL_ID_BITS_COUNT                                    (5)
#define USER_PACKET_SIZE_BITS_COUNT       (32-USER_PACKET_CHANNEL_ID_BITS_COUNT)
#define GET_USER_HEADER_SIZE(iBits) (iBits & ((1 << USER_PACKET_SIZE_BITS_COUNT) - 1))
#define GET_USER_HEADER_CHANNEL_ID(iBits) ((iBits >> USER_PACKET_SIZE_BITS_COUNT) & ((1 << USER_PACKET_CHANNEL_ID_BITS_COUNT) - 1))

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

enum eP7Trace_Level
{
    EP7TRACE_LEVEL_TRACE        = 0,
    EP7TRACE_LEVEL_DEBUG           ,
    EP7TRACE_LEVEL_INFO            ,
    EP7TRACE_LEVEL_WARNING         ,
    EP7TRACE_LEVEL_ERROR           ,
    EP7TRACE_LEVEL_CRITICAL        ,

    EP7TRACE_LEVEL_COUNT
};

enum eClient_Status
{
    //Regular statuses
    ECLIENT_STATUS_OK                   = 0,
    ECLIENT_STATUS_OFF                     ,
    ECLIENT_STATUS_INTERNAL_ERROR          ,

    //Temporary statuses
    ECLIENT_STATUS_DISCONNECTED            ,
    ECLIENT_STATUS_NO_FREE_BUFFERS         ,
    ECLIENT_STATUS_NOT_ALLOWED             ,
    ECLIENT_STATUS_WRONG_PARAMETERS        ,
    ECLIENT_STATUS_WRONG_FORMAT
};

enum eTrace_Arg_Type
{
    P7TRACE_ARG_TYPE_UNK    = 0x00,
    P7TRACE_ARG_TYPE_CHAR   = 0x01,
    P7TRACE_ARG_TYPE_INT8   = 0x01,
    P7TRACE_ARG_TYPE_CHAR16  ,//(0x02)
    P7TRACE_ARG_TYPE_INT16   ,//(0x03)
    P7TRACE_ARG_TYPE_INT32   ,//(0x04)
    P7TRACE_ARG_TYPE_INT64   ,//(0x05)
    P7TRACE_ARG_TYPE_DOUBLE  ,//(0x06)
    P7TRACE_ARG_TYPE_PVOID   ,//(0x07)
    P7TRACE_ARG_TYPE_USTR16  ,//(0x08) //unicode - UTF16 string
    P7TRACE_ARG_TYPE_STRA    ,//(0x09) //ASCII string
    P7TRACE_ARG_TYPE_USTR8   ,//(0x0A) //unicode - UTF8 string
    P7TRACE_ARG_TYPE_USTR32  ,//(0x0B) //unicode - UTF32 string
    P7TRACE_ARG_TYPE_CHAR32  ,//(0x0C)
    P7TRACE_ARG_TYPE_INTMAX  ,//(0x0D)

    P7TRACE_ARGS_COUNT
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

enum eTPacket_Type
{
    ETPT_CLIENT_HELLO             = 0x0,    //tH_Common + tH_Client_Initial
    ETPT_CLIENT_PING              = 0x1,    //tH_Common
    ETPT_CLIENT_DATA              = 0x2,
    ETPT_CLIENT_DATA_REPORT       = 0x3,
    ETPT_CLIENT_BYE               = 0x4,

    ETPT_ACKNOWLEDGMENT           = 0x9,
    ETPT_SERVER_DATA_REPORT       = 0xA,
    ETPT_SERVER_CHAINED           = 0xB,

    ETPACKET_TYPE_UNASSIGNED      = 0xF
};

enum eP7User_Type
{
    EP7USER_TYPE_TRACE          =  0,
    EP7USER_TYPE_TELEMETRY_V1       ,
    EP7USER_TYPE_TELEMETRY_V2       ,

    EP7USER_TYPE_MAX            = P7_EXTENSION_MAX_TYPES
};

#pragma pack(push,2)
struct sH_Client_Hello
{
    tUINT16  wProtocol_Version;
    tUINT16  wData_Max_Size;
    //Process ID and process time are dedicated to matching processes on server
    //side
    tINT32  dwProcess_ID;
    tINT32  dwProcess_Start_Time_Hi;
    tINT32  dwProcess_Start_Time_Lo;
    tWCHAR   pProcess_Name[96];
};

//Пока что значения внутри следующих структур взяты на основе изучений вайршарка, потом надо будет сделать реализации внутри!!!
struct sH_Packet_Header
{
    tINT32  dwCRC32;
    tINT32  dwID;
    tUINT16  wBits = 73; //[0..3] - Type, [4..15] - flags
    tUINT16  wSize = 36;
    tUINT16  wClient_ID = 0;
};

struct sH_Packet_Ack
{
    tINT32  dwSource_ID = 1;
    tUINT16  wResult = 1; // 0 - NOK, 1 - OK
};


struct sH_Ext
{
    tUINT16  wType = 0;  //extension type, тип дополнения берется из enum eTPacket_Extension
    tUINT16  wSize = 16;  //extension size, размер дополнения
};


struct sH_Ext_Srv_Info
{
    tINT32  dwServerVersion = 0; //Server version
    tUINT16  wProtocolVersion = 7;//Server protocol version
    tUINT16  wMTU = 0;            //maximum transfer unit (max packet size passed by all network route devices), 0 if unknown
    tINT32  dwSocket_Buffer = 1471; //size of recv socket buffer in bytes
};


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
    tINT32 argsLen;
    std::vector<tUINT16> argsID;
    std::vector<tUINT64> argsValue;
    tINT8* endPoint;
};

struct MapDataStruct
{
    sP7Trace_Format traceFormat;
    TraceLineData traceDataPerLine;
    sP7Trace_Data traceData;
};

struct sP7Tel_Info
{
    tUINT32       sP7Ext_Raw;

    tUINT32       dwTime_Hi;
    tUINT32       dwTime_Lo;

    tUINT64       qwTimer_Value;
    //timer's count heartbeats in second
    tUINT64       qwTimer_Frequency;
    tUINT64       qwFlags;
    tWCHAR        pName[P7TELEMETRY_NAME_LENGTH];
};

struct sP7Tel_Counter_v1
{
    tUINT32       sP7Ext_Raw;
    tUINT8        bID;
    tUINT8        bOn;
    tINT64        llMin;
    tINT64        llMax;
    tINT64        llAlarm;
    tWCHAR        pName[P7TELEMETRY_COUNTER_NAME_LENGTH_V1];
};


//Telemetry counter description
struct sP7Tel_Counter_v2
{
    tUINT32       sP7Ext_Raw;

    tUINT16  wID;
    tUINT16  bOn;
    tDOUBLE  dbMin;
    tDOUBLE  dbAlarmMin;
    tDOUBLE  dbMax;
    tDOUBLE  dbAlarmMax;
    tWCHAR   pName[2];
};

struct sP7Tel_Enable_v1
{

    tUINT32       sP7Ext_Raw;
    tUINT8 bID;
    tUINT8 bOn;
};

struct sP7Tel_Enable_v2
{
    tUINT32       sP7Ext_Raw;
    tUINT16 wID;
    tUINT16 bOn;
};
struct sP7Tel_Value_v1
{
    tUINT32       sP7Ext_Raw;
    tUINT8  bID;
    tUINT8  bSeqN;
    tUINT64 qwTimer;      //High resolution timer value
    tINT64  llValue;
};

struct sP7Tel_Value_v2
{
    tUINT32       sP7Ext_Raw;
    tUINT16       wID;
    tUINT16       wSeqN;
    tUINT64       qwTimer;      //High resolution timer value
    tDOUBLE       dbValue;
};

struct sP7Tel_Utc_Offs_V2
{
    tUINT32       sP7Ext_Raw;
    tINT32       iUtcOffsetSec;
};
#pragma pack(pop)

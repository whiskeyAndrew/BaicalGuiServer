#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H
#pragma comment (lib, "ws2_32.lib")
#include <winsock2.h>
#include "PCRC32.h"
#include "chunkhandler.h"

#define CRC_OFFSET (4)
#define UDP_HEAD_OFFSET (14)

#define USER_PACKET_CHANNEL_ID_BITS_COUNT                                    (5)
#define USER_PACKET_SIZE_BITS_COUNT       (32-USER_PACKET_CHANNEL_ID_BITS_COUNT)
#define GET_USER_HEADER_SIZE(iBits) (iBits & ((1 << USER_PACKET_SIZE_BITS_COUNT) - 1))

class Launcher;
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
#pragma pack(push,2)
struct BufferData
{
    tUINT8* packetPointer;
    tUINT32 sizeBuffer;
};

struct sH_Packet_Header
{
    tUINT32  dwCRC32;
    tUINT32  dwID;
    tUINT16  wBits = 73; //[0..3] - Type, [4..15] - flags
    tUINT16  wSize = 36;
    tUINT16  wClient_ID = 0;
};


struct sH_Client_Hello
{
    tUINT16  wProtocol_Version;
    tUINT16  wData_Max_Size;
    //Process ID and process time are dedicated to matching processes on server
    //side
    tUINT32  dwProcess_ID;
    tUINT32  dwProcess_Start_Time_Hi;
    tUINT32  dwProcess_Start_Time_Lo;
    tWCHAR    pProcess_Name[96];
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
#pragma pack(pop)

class PacketHandler:public QThread
{
    Q_OBJECT
private:
    time_t lastPacketTime;
    Launcher* launcher;

    SOCKET socketIn;
    QMutex mutex;
    tUINT8* queueElement;
    tUINT8* packetBuffer;
    tUINT8* packetCursor;
    tUINT8* packetEnd;
    tINT8* tempBuffer;

    tINT8* chunkBuffer;
    tINT8* p_chunkPointer;

    tINT32 bytesTransfered = 0;
    tUINT32 packetSize;
    tUINT32 packetType;
    tUINT32 lastPacket;
    tUINT32 packetCRC;
    tUINT32 counterPacketsFromServer = 0;
    tUINT32 bytesLeft;
    tUINT32 chunkSize = 0;

    std::vector<char>dataVector;
    std::vector<char> bufferVector;

    BufferData dataFromQueue;
    sH_Packet_Header packetHeader;
    sH_Client_Hello packetHello;

    //Структурки для ответа клиенту
    sH_Packet_Header outPacketHeader;
    sH_Packet_Ack outPacketAck;
    sH_Ext outPacketExt;
    sH_Ext_Srv_Info outPacketUDPInfo;

    bool GetPacketFromQueue();
    bool HandleHelloPacket();
    bool HandleReportPacket();
    bool HandlePingPacket();
    bool PacketProcessing();

    tUINT32 GetPacketType(sH_Packet_Header pckHdr);
    ~PacketHandler();
    void run();

public:
    QMutex syncThreads;
    QWaitCondition waitCondition;

    PacketHandler();
    ChunkHandler chunkHandler;
    std::queue<std::vector<tINT8>> packetQueue;
    std::vector<tINT8> tempVector;

    sockaddr_in client;

    PacketHandler(sockaddr_in client, Launcher* launcher)
    {
        this->client = client;
        this->launcher = launcher;
    }

    //Вызываем чтобы заполнять очередь внутри потока пакетами
    void AppendQueue(tUINT8* bufferPointer, tUINT32 bufferSize);


    void setSocketIn(SOCKET newSocketIn);
    bool InitData();

    time_t getLastPacketTime();

signals:
    void ConnectionLost(sockaddr_in client);

};

#endif // PACKETHANDLER_H

#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H
#include <QThread>
#include <queue>
#include <QMutex>
#pragma comment (lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

struct BufferData
{
    char* packetPointer;
    int sizeBuffer;
};

struct sH_Packet_Header
{
    int  dwCRC32;
    int  dwID;
    short  wBits = 73; //[0..3] - Type, [4..15] - flags
    short  wSize = 36;
    short  wClient_ID = 0;
};

class PacketHandler:public QThread
{
private:
    QMutex mutex;
    char *queueElement;
    char* packetBuffer;
    char* packetCursor;
    int packetSize;
    int packetType;
    int lastPacketNumber;

    std::queue<BufferData> packetQueue;
    BufferData dataFromQueue;
    sH_Packet_Header packetHeader;

    void GetPacketFromQueue();
    bool PacketProcessing();
    int GetPacketType(sH_Packet_Header pckHdr);
public:
    PacketHandler();

    sockaddr_in client;
    PacketHandler(sockaddr_in clientConstr)
    {
        client = clientConstr;
    }

    //Вызываем чтобы заполнять очередь внутри потока пакетами
    void AppendQueue(char* bufferPointer, int bufferSize);

    void run();
};

#endif // PACKETHANDLER_H

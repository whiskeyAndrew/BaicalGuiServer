#ifndef LAUNCHER_H
#define LAUNCHER_H
#define CRC_OFFSET (4)
#define UDP_HEAD_OFFSET (14)

#pragma comment (lib, "ws2_32.lib")
#include <winsock2.h>
#include "mainwindow.h"
#include "packethandler.h"
#include <string.h>
#include <queue>
#include <iostream>
#include <QVector>
#include <QMap>
#include <QMutex>
#include <QList>
#include <QApplication>
#include <QDebug>
#include "serverstatussender.h"
#include "debuglogger.h"

//std::vector<char> vectorChunk;
//std::queue<std::vector<char>> queueChunks;
#pragma pack(push,2)
struct ClientData
{
    sockaddr_in clientIp;
    PacketHandler* connectionThread;
};
#pragma pack(pop)

class ConnectionTimeoutChecker;
class Launcher:public QThread
{
    Q_OBJECT
private:
    //UDP Protocol
    bool socketStarted = false;
    QMutex mutex;
    WSADATA data;
    WORD version = MAKEWORD(2, 2);
    tUINT32 bytesIn;

    tUINT32 iOptVal = 0x200000; //2мб
    tUINT32 iOptLen = sizeof (tUINT32);

    //Инициализирует запуск winsock, возввращает ноль если запуск прошел успешно
    tUINT32 winsockStatus;

    SOCKET socketIn;
    MainWindow* mainWindow;
    //Клиент
    sockaddr_in serverHint;
    sockaddr_in client;
    static const tUINT32 clientListSize = 30;

    PacketHandler* packetHandler;

    tUINT32 clientLength = sizeof(client);
    tUINT8* ipClient;
    tUINT32 portClient;

    //Буффер пакета
    tUINT8 packetBuffer[65280]; //65280 - максимальный размер входящих даных

    ConnectionTimeoutChecker* connectionTimeoutChecker;
    bool initSocket();
    void listenSocket();
    void run();

public:
    QList<ClientData>* clientsList;
    Launcher(MainWindow* mw);

    bool findClientInArray();

public slots:
    void deleteClient(tUINT32 clientNumber);
signals:
    void sendNewConnection(sockaddr_in newConnection,PacketHandler* packetHandler);
    void changeClientStatus(sockaddr_in client);

};

#endif // LAUNCHER_H

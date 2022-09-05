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

//std::vector<char> vectorChunk;
//std::queue<std::vector<char>> queueChunks;
struct ClientData
{
    sockaddr_in clientIp;
    PacketHandler *connectionThread;
};



class Launcher:public QThread
{
    Q_OBJECT
public:
    Launcher()
    {

    }
private:
    //UDP Protocol
    bool socketStarted = false;

    WSADATA data;
    WORD version = MAKEWORD(2, 2);
    int bytesIn;

    int iOptVal = 0x200000; //2мб
    int iOptLen = sizeof (int);

    //Инициализирует запуск winsock, возввращает ноль если запуск прошел успешно
    int winsockStatus;

    SOCKET socketIn;
    MainWindow *mainWindow;
    //Клиент
    sockaddr_in serverHint;
    sockaddr_in client;
    static const int clientListSize = 12;
    ClientData clientsList[clientListSize];
    PacketHandler *packetHandler;

    int clientLength = sizeof(client);
    char* ipClient;
    int portClient;

    //Буффер пакета
    char packetBuffer[65280]; //65280 - максимальный размер входящих даных

public:
    void run();
    bool FindClientInArray();
    bool InitSocket();
    void SocketListener();
    void SetMainWindow(MainWindow *mw)
    {
        mainWindow = mw;
    }

signals:
    void SendNewConnection(sockaddr_in newConnection);
};

#endif // LAUNCHER_H

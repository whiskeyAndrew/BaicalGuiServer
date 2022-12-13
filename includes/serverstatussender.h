#ifndef SERVERSTATUSSENDER_H
#define SERVERSTATUSSENDER_H

#include <QThread>
#include <iostream>
#include <winsock2.h>
#include <string>
using namespace std;

#pragma comment (lib, "ws2_32.lib")

#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define BUFLEN 150  // max length of answer
#define PORT 9019  // the port on which to listen for incoming data
//DELETE AFTER PROD DEPLOY

class Launcher;

class ServerStatusSender: public QThread
{
public:
    ServerStatusSender(Launcher* newLauncher);
private:
    Launcher* launcher;
    void run();
    sockaddr_in server;
    WSADATA ws;
    int client_socket;

};

#endif // SERVERSTATUSSENDER_H

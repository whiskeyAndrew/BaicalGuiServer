#include "serverstatussender.h"
#include "launcher.h"
//DELETE AFTER PROD DEPLOY

ServerStatusSender::ServerStatusSender(Launcher *newLauncher)
{
    launcher = newLauncher;
tryingAgain:

    system("title UDP Client");

    // initialise winsock
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        goto tryingAgain;
    }
    printf("Initialised.\n");

    // create socket

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
    {
        printf("socket() failed with error code: %d", WSAGetLastError());
        goto tryingAgain;
    }

    // setup address structure
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);
}


void ServerStatusSender::run()
{
    tUINT32 msgNumber = 0;
    while (true)
    {
        QString msg = "";


        for(int i =0;i<launcher->clientsList->size();i++){
            msg.append(inet_ntoa(launcher->clientsList->at(i).clientIp.sin_addr));
            msg.append("#"+QString::number(++msgNumber));
            msg.append("#200");
            msg.append("#packetHandler queue size:"+QString::number(launcher->clientsList->at(i).connectionThread->packetQueue.size()));
            msg.append("#chunkHandler queue size:"+QString::number(launcher->clientsList->at(i).connectionThread->chunkHandler.chunks.size()));
            msg.append("\n ");
        }
//        msg.insert(0,msg.size());
        if(launcher->clientsList->size()==0){
//            msg = "no connections";
            continue;
        }
        msg.append("\0");
        char message[BUFLEN];
        strcpy(message,msg.toStdString().c_str());

        // send the message
        if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code: %d", WSAGetLastError());
            continue;
        }

        // receive a reply and print it
        // clear the answer by filling null, it might have previously received data
        char answer[BUFLEN] = {};

        // try to receive some data, this is a blocking call
        int slen = sizeof(sockaddr_in);
        int answer_length;

        //        cout<<"sended"<<endl;
        this->sleep(1);
    }

    closesocket(client_socket);
    WSACleanup();
}

#include "launcher.h"

void Launcher::run()
{
    connect(this,&Launcher::SendNewConnection,mainWindow,&MainWindow::GetNewConnection);
    connect(this,&Launcher::ChangeClientStatus,mainWindow,&MainWindow::ChangeClientStatus);
    std::cout<<"Launcher:: Started socket init"<<std::endl;
    //чистим при старте список клиентов
    memset(clientsList,0,sizeof(clientsList));
    //Инициализируем сокет
    while(!socketStarted)
    {
        std::cout<<"Launcher:: Trying to init socket"<<std::endl;
        socketStarted = InitSocket();
    }

    //Сокет инициализирован, начинаем слушать порт
    std::cout<<"Launcher:: Socket is ready!"<<std::endl;
    std::cout<<"Launcher:: Start listening for incoming connections"<<std::endl;

    while(true)
    {
        SocketListener();
    }

}

bool Launcher::InitSocket()
{
    winsockStatus = WSAStartup(version, &data);

    if(winsockStatus!=0)
    {
        std::cout<< "Launcher:: can't start winsock! Error: " <<winsockStatus<<std::endl;
        return false;
    }

    socketIn = socket(AF_INET, SOCK_DGRAM,0);
    if (socketIn == INVALID_SOCKET)
    {
        std::cout << "Can't create a socket! Quitting" << std::endl;
        return false;
    }
    setsockopt(socketIn,SOL_SOCKET,SO_RCVBUF,(char*)&iOptVal, iOptLen);
    serverHint.sin_addr.S_un.S_addr = INADDR_ANY; //IP, указываем что можно использовать любую сетевую карту на машине
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(9009); //Convert from LE to BE, PORT

    if(::bind(socketIn, (sockaddr*)&serverHint,sizeof(serverHint))==SOCKET_ERROR)
    {
        std::cout << "Launcher:: Can't bind socket! Error: " << WSAGetLastError() << std::endl;
        std::cout<< "Restarting socket..." <<std::endl;
        winsockStatus = closesocket(socketIn);
        if(winsockStatus!=0)
        {
            std::cout<<"WSACleanum falied! Error: " <<winsockStatus<<std::endl;
        }
        return false;
    }

    listen(socketIn,SOMAXCONN);
    //Создаем место для буфера клиента, сюда помещается информация о клиенте
    ZeroMemory(&client, sizeof(client));
    return true;
}

void Launcher::SocketListener()
{
    //Принимаем пакет
    bytesIn = recvfrom(socketIn, (tINT8*)packetBuffer, sizeof(packetBuffer), 0, (sockaddr*)&client, (tINT32*)&clientLength);
    if(bytesIn==SOCKET_ERROR)
    {
        std::cout<<"Launcher:: recvfrom failed, error: "<<WSAGetLastError()<<std::endl;
        return;
    }

    //Отправляем пакет в обработчик пакетов, параллельно проверяем есть ли у нас такие клиенты
    bool clientArrayStatus = FindClientInArray();

    //если что-то пошло не так при добавления клиента в массив
    if(clientArrayStatus==0)
    {
        std::cout<<"Error in clientsArray!";
        Sleep(500000); //пока что временно
    }
}

void Launcher::ConnectionLost(sockaddr_in client){

    emit ChangeClientStatus(client);
}

bool Launcher::FindClientInArray()
{
    //Везде накидываем нтохсы потому что так правильно будет выводить пришедший порт
    //Ищем клиента в массиве клиентов, если он есть
    for(int i =0;i<clientListSize;i++)
    {
        if((ntohs(client.sin_addr.S_un.S_addr) == ntohs(clientsList[i].clientIp.sin_addr.S_un.S_addr))
                && (ntohs(client.sin_port) == ntohs(clientsList[i].clientIp.sin_port)))
        {
            packetHandler = clientsList[i].connectionThread;
            packetHandler->AppendQueue(packetBuffer,bytesIn);

            return true;
        }
    }

    //Не нашли клиента, делаем нового
    //Пока что проверяем на наличие пустых слотов, потом перезапись сдаелам
    for(int i =0;i<clientListSize;i++)
    {
        if(clientsList[i].clientIp.sin_addr.S_un.S_addr == 0)
        {
            clientsList[i].clientIp = client;
            PacketHandler *packetHandler = new PacketHandler(client,this);
            packetHandler->AppendQueue(packetBuffer,bytesIn);
            packetHandler->setSocketIn(socketIn);
            clientsList[i].connectionThread = packetHandler;

            clientsList[i].connectionThread->start();
            emit SendNewConnection(client);
            return true;
        }
    }
    return false;
}

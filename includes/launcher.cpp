#include "launcher.h"
#include "connectiontimeoutchecker.h"

//от названия Лаунчер надо было отказаться, потому что
//по сути это класс, который слушает и отправляет в packetHandler пакеты, полученные по UDP сокету
//для начала сокет инициализируется, после чего начинает бесконечную прослушку по порту 9009
//все клиенты хранятся в ClientData (обрезок нормального класса клиента, который стоило бы сделать в самом начале написания проекта, я обязательно когда-нибудь перепишу бэк этого приложения)
//получил пакет -> если клиент новый, то создает нового клиента и помещает в список клиентов, затем создается packetHandler и затем все пакеты от этого клиента передаются в него

void Launcher::run()
{
    connect(this,&Launcher::sendNewConnection,mainWindow,&MainWindow::getNewConnection);

    clientsList = new QList<ClientData>;
    connectionTimeoutChecker = new ConnectionTimeoutChecker(mainWindow);
    connectionTimeoutChecker->start();

    std::cout<<"Launcher:: Started socket init"<<std::endl;

    //Инициализируем сокет
    while(!socketStarted)
    {
        std::cout<<"Launcher:: Trying to init socket"<<std::endl;
        socketStarted = initSocket();
    }

    //Сокет инициализирован, начинаем слушать порт
    std::cout<<"Launcher:: Socket is ready!"<<std::endl;

    std::cout<<"Launcher:: Start listening for incoming connections"<<std::endl;

    while(!this->isInterruptionRequested())
    {
        listenSocket();
    }
    connectionTimeoutChecker->requestInterruption();
    connectionTimeoutChecker->wait();

    for(int i =0;i<clientsList->size();i++){
        if(clientsList->at(i).connectionThread!=NULL){
            clientsList->at(i).connectionThread->requestInterruption();
        }

    }
    for(int i =0;i<clientsList->size();i++){
        if(clientsList->at(i).connectionThread!=NULL){
            clientsList->at(i).connectionThread->wait();
        }
    }
    std::cout<<"------"<<"Launcher is ending"<<"------"<<std::endl;
    QApplication::quit();
}

bool Launcher::initSocket()
{
    winsockStatus = WSAStartup(version, &data);

    if(winsockStatus!=0)
    {
        std::cout<< "Launcher:: can't start winsock! Error: " <<winsockStatus<<std::endl;
        return false;
    }

    socketIn = socket(AF_INET, SOCK_DGRAM,0);
    DWORD timeout = 1000;
    setsockopt(socketIn, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
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

void Launcher::listenSocket()
{
    //Принимаем пакет
    bytesIn = recvfrom(socketIn, (tINT8*)packetBuffer, sizeof(packetBuffer), 0, (sockaddr*)&client, (tINT32*)&clientLength);
    if(WSAGetLastError()==10060){
        std::cout<<"Launcher:: no packets to read from UDP socket, trying again... "<<std::endl;
        return;
    }
    else if(bytesIn==SOCKET_ERROR)
    {
        std::cout<<"Launcher:: recvfrom failed, error: "<<WSAGetLastError()<<std::endl;
        return;
    }

    //Отправляем пакет в обработчик пакетов, параллельно проверяем есть ли у нас такие клиенты
    bool clientArrayStatus = findClientInArray();

    //если что-то пошло не так при добавления клиента в массив
    if(clientArrayStatus==0)
    {
        std::cout<<"Error in clientsArray!";
        Sleep(500000); //пока что временно
    }
}

Launcher::Launcher(MainWindow *mw)
{
    mainWindow = mw;
}

void Launcher::deleteClient(tUINT32 clientNumber)
{
    if(clientsList->at(clientNumber).connectionThread!=NULL){
        connectionTimeoutChecker->removeClientAt(clientNumber);
    }
    clientsList->removeAt(clientNumber);
    std::cout<<"deleted client at "<<clientNumber<<std::endl;
}

bool Launcher::findClientInArray()
{
    //Надо полностью переписать структуру клиентов, она идеально работает, но написана через жёпу настолько, что я сам забыл как оно работает

    //Везде накидываем нтохсы потому что так правильно будет выводить пришедший порт
    //Ищем клиента в массиве клиентов, если он есть
    for(int i =0;i<clientsList->size();i++)
    {
        if((ntohs(client.sin_addr.S_un.S_addr) == ntohs(clientsList->at(i).clientIp.sin_addr.S_un.S_addr))
                && (ntohs(client.sin_port) == ntohs(clientsList->at(i).clientIp.sin_port)))
        {
            packetHandler = clientsList->at(i).connectionThread;

            //внезапно ловим соединение которое уже было, но с тем же самым айпишником и портом, проверяем по состоянию потока обработки пакетов
            //не уверен что работает, но в теории должно, надеюсь?
            if(packetHandler->isFinished() || packetHandler->isInterruptionRequested()){
                //Если приходит какое-то соединение с тем же айпишником и портом, а до этого у нас уже поток завершился, то скипаем существующий и тогда приложение создаст новый
                //Если придет одинаковый айпишник и одинаковый порт на активные соединения то будем считать что это аномалия
                //Но на самом деле в теории приложение будет оба обрабатывать как одно и есть риск захлебнуться если данных будет слишком много
                continue;
            }
            packetHandler->appendQueue(packetBuffer,bytesIn);
            packetHandler->waitCondition.wakeAll();

            return true;
        }
    }

    //Не нашли клиента, делаем нового
    PacketHandler* newPacketHandler = new PacketHandler(client);
    clientsList->append({client,newPacketHandler});
    connectionTimeoutChecker->appendClientsMap({client,newPacketHandler});

    emit sendNewConnection(client,newPacketHandler);

    newPacketHandler->appendQueue(packetBuffer,bytesIn);
    newPacketHandler->setSocketIn(socketIn);
    return true;

}

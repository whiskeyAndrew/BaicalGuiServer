#include "packethandler.h"
#include "launcher.h"


void PacketHandler::run()
{
    //Нам надо один раз запустить и запомнить поток обработки чанков, это мы сделаем здесь
    chunkHandler.start();
    connect(this,&PacketHandler::ConnectionLost,launcher,&Launcher::ConnectionLost);

    while(true)
    {
        //Берем из очереди буфер

        if(!GetPacketFromQueue()){
            continue;
        }
        //Обрабатываем
        if(!PacketProcessing())
        {
            //std::cout<<"Getting next packet" <<std::endl;
            free(packetBuffer);
            continue;
        }
        free(packetBuffer);
    }
}

void PacketHandler::AppendQueue(tUINT8* bufferPointer, tUINT32 bufferSize){
    //Дополняем очередь packetBuffer-а
    mutex.tryLock(-1);
    BufferData bufferData{bufferPointer,bufferSize};
    std::vector<tINT8> tempVector;
    tempVector.resize(bufferSize);
    tINT8* tempPointer = tempVector.data();
    memcpy(tempPointer,bufferPointer,bufferSize);
    packetQueue.push(tempVector);
    mutex.unlock();
}

void PacketHandler::setSocketIn(SOCKET newSocketIn)
{
    socketIn = newSocketIn;
}

bool PacketHandler::GetPacketFromQueue()
{
    //Ждем пока очередь чем-то заполнится, НА ВСЯКИЙ СЛУЧАЙ

//    int time = GetTickCount();

    if(packetQueue.empty()){
        syncThreads.tryLock(-1);
        waitCondition.wait(&syncThreads);
        syncThreads.unlock();
    }

//    while(packetQueue.empty())
//    {
//        //Перед заходом в цикл берем момент захода в метод, если проход 5 секунд, а пакетов не появляется - значит соединение разорвано
//        if(GetTickCount()>time+5000){
//            std::cout<<"Lost connection"<<std::endl;
//            emit ConnectionLost(client);
//            delete this;
//        }
//    }

    //Сюда пришли если очередь не пуста, откусываем и начинаем обработку
    if(packetQueue.empty()){
        return false;
    }
    mutex.tryLock(-1);
    tempVector = packetQueue.front();
    packetQueue.pop();
    mutex.unlock();
    //передаем в локальные переменные чтобы не мучатьь себе голову на протяжении всего кода из-за структуры
    packetSize = tempVector.size();
    packetBuffer = (tUINT8*)malloc(packetSize);
    memcpy(packetBuffer,tempVector.data(),packetSize);
    packetCursor = packetBuffer;
    return true;
}

bool PacketHandler::PacketProcessing()
{
    //Читаем заголовок пакета и перескакиваем его
    memcpy(&packetHeader,packetCursor,sizeof(sH_Packet_Header));
    packetSize = packetHeader.wSize;
    packetCursor+=14;

    packetType = GetPacketType(packetHeader);
    lastPacket = packetHeader.dwID;

    switch(packetType)
    {
    case ETPT_CLIENT_DATA:
    {
        bool dataReady = InitData();

        if(dataReady==FALSE)
        {
            return false;
        }
        break;
    }


    case ETPT_CLIENT_DATA_REPORT:
    {
        if(!HandleReportPacket())
        {
            return false;
        }
        break;
    }
    case ETPT_CLIENT_PING:
    {
        if(!HandlePingPacket())
        {
            return false;
        }
        break;
    }
    case ETPT_CLIENT_HELLO:
    {
        if(!HandleHelloPacket())
        {
            return false;
        }
        break;
    }
    case ETPT_CLIENT_BYE:
    {
        break;
    }
    default:
    {
        std::cout<<"Can't get packet type"<<std::endl;
        return false;
        break;
    }
        return true;
    }

    return true;
}

bool PacketHandler::InitData()
{
    bytesLeft = packetSize-UDP_HEAD_OFFSET;

    for(int i = 0;i<bytesLeft;i++)
    {
        dataVector.push_back(*packetCursor);
        packetCursor++;
    }

newChunk:
    if(chunkSize==0)
    {
        memcpy(&chunkSize,dataVector.data(),sizeof(tUINT32));
        chunkSize = GET_USER_HEADER_SIZE(chunkSize);
    }
    if(bytesLeft-chunkSize<0)
        return false;

    bytesLeft-=chunkSize;

    //std::cout<<chunkSize<<std::endl;

    while(chunkSize<dataVector.size())
    {
        for(int i =0;i<chunkSize;i++)
        {
            bufferVector.push_back(*dataVector.data());
            dataVector.erase(dataVector.begin());
        }

        //Передаем в очередь наш вектор
        chunkHandler.AppendChunksQueue(bufferVector);

        chunkHandler.syncThreads.tryLock(-1);
        chunkHandler.waitCondition.wakeAll();
        chunkHandler.syncThreads.unlock();

        bufferVector.clear();//На всякий случай
        chunkSize=0;

        if(bytesLeft==0)
            return true;
        goto newChunk;
    }
    return false;
}



tUINT32 PacketHandler::GetPacketType(sH_Packet_Header pckHdr)
{
    return (packetHeader.wBits &0xFu);
}

PacketHandler::~PacketHandler()
{
    this->terminate();
}

bool PacketHandler::HandleHelloPacket()
{
    memcpy(&packetHello,packetCursor,sizeof(sH_Client_Hello));
    packetEnd = packetCursor+sizeof(sH_Client_Hello);

    packetCRC = Get_CRC32((unsigned char*)packetBuffer + CRC_OFFSET, packetSize-CRC_OFFSET);
    if(packetCRC!=packetHeader.dwCRC32)
    {
        std::cout<<"CRC fucked up"<<std::endl;
        return false;
    }

    chunkHandler.InitBackupWriter(packetHello.dwProcess_ID,
                                  packetHello.dwProcess_Start_Time_Hi,
                                  packetHello.dwProcess_Start_Time_Lo);

    counterPacketsFromServer++;
    outPacketHeader.dwID = counterPacketsFromServer;
    //Начинаем генерацию ответа на приветственный пакет
    tUINT32 outputPacketSize = sizeof(sH_Packet_Header) + sizeof(sH_Packet_Ack) + sizeof(sH_Ext) + sizeof(sH_Ext_Srv_Info);
    tINT8* outputBuf = (tINT8*)malloc(outputPacketSize);
    tINT8* outputBufCursor = outputBuf;

    //Пишем в ответ заголовок пакета
    memcpy(outputBufCursor,&outPacketHeader,sizeof(sH_Packet_Header));
    outputBufCursor+=sizeof(sH_Packet_Header);

    //Пишем структуру
    outPacketAck.dwSource_ID = lastPacket;
    memcpy(outputBufCursor,&outPacketAck,sizeof(sH_Packet_Ack));
    outputBufCursor+=sizeof(sH_Packet_Ack);

    //Пишем структуру
    memcpy(outputBufCursor,&outPacketExt,sizeof(sH_Ext));
    outputBufCursor+=sizeof(sH_Ext);

    //Пишем структуру Опять
    memcpy(outputBufCursor,&outPacketUDPInfo,sizeof(sH_Ext_Srv_Info));

    //Подсчитываем CRC у полученного буфера и высылаем клиенту
    packetCRC = Get_CRC32((tUINT8*)outputBuf + CRC_OFFSET, outputPacketSize-CRC_OFFSET);
    memcpy(outputBuf,&packetCRC,sizeof(tUINT32));

    tUINT32 bytesOut = sendto(socketIn, outputBuf, outputPacketSize, 0, (sockaddr*)&client, sizeof(client));
    free(outputBuf);
    if (bytesOut == SOCKET_ERROR) {
        std::cout<<"Can't send Hello responce, error: "<< WSAGetLastError()<<std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

bool PacketHandler::HandleReportPacket()
{
    counterPacketsFromServer++;
    // Надо разобрать как генерируется ответ клиенту чтобы он не был статический, будем разбираться
    sH_Packet_Header reportAnswer = {0,counterPacketsFromServer,9,20,0};
    short serverStatus = 1;
    tINT8* bufferReportAnswer = (tINT8*)malloc(20);
    memcpy(bufferReportAnswer,&reportAnswer,sizeof(sH_Packet_Header));
    bufferReportAnswer+=sizeof(sH_Packet_Header);
    memcpy(bufferReportAnswer,&lastPacket,sizeof(tUINT32));
    bufferReportAnswer+=sizeof(tUINT32);
    memcpy(bufferReportAnswer,&serverStatus,sizeof(tUINT16));
    bufferReportAnswer-=18;
    packetCRC = Get_CRC32((tUINT8*)bufferReportAnswer + CRC_OFFSET, 20-CRC_OFFSET);
    memcpy(bufferReportAnswer,&packetCRC,sizeof(tUINT32));
    tUINT32 bytesOut = sendto(socketIn, bufferReportAnswer, 20, 0, (sockaddr*)&client, sizeof(client));
    free(bufferReportAnswer);

    if (bytesOut == SOCKET_ERROR  || bytesOut==0) {
        std::cout<<"Can't send DataReport responce, error: "<< WSAGetLastError()<<std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

bool PacketHandler::HandlePingPacket()
{
    counterPacketsFromServer++;
    // Надо разобрать как генерируется ответ клиенту чтобы он не был статический, будем разбираться
    sH_Packet_Header reportAnswer = {0,counterPacketsFromServer,9,20,0};
    short serverStatus = 1;
    tINT8*bufferReportAnswer = (tINT8*)malloc(20);
    memcpy(bufferReportAnswer,&reportAnswer,sizeof(sH_Packet_Header));
    bufferReportAnswer+=sizeof(sH_Packet_Header);
    memcpy(bufferReportAnswer,&lastPacket,sizeof(tUINT32));
    bufferReportAnswer+=sizeof(tUINT32);
    memcpy(bufferReportAnswer,&serverStatus,sizeof(tUINT16));
    bufferReportAnswer-=18;
    packetCRC = Get_CRC32((tUINT8*)bufferReportAnswer + CRC_OFFSET, 20-CRC_OFFSET);
    memcpy(bufferReportAnswer,&packetCRC,sizeof(tUINT32));
    tUINT32 bytesOut = sendto(socketIn, bufferReportAnswer, 20, 0, (sockaddr*)&client, sizeof(client));
    free(bufferReportAnswer);

    if (bytesOut == SOCKET_ERROR) {
        std::cout<<"Can't send DataReport responce, error: "<< WSAGetLastError()<<std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

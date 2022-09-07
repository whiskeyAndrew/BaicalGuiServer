#include "packethandler.h"



void PacketHandler::run()
{
    //Нам надо один раз запустить и запомнить поток обработки чанков, это мы сделаем здесь

    chunkHandler.start();


    while(true)
    {

        //Туду: здесь должен быть обработчик пакетов. Нужно брать из очереди буфер и обрабатывать его так же, как это было в варианте с одним соединением
        //Берем из очереди буфер
        GetPacketFromQueue();
        //Обрабатываем
        if(!PacketProcessing())
        {
            std::cout<<"Getting next packet" <<std::endl;
            continue;
        }
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

void PacketHandler::GetPacketFromQueue()
{
    //Ждем пока очередь чем-то заполнится, НА ВСЯКИЙ СЛУЧАЙ
    while(packetQueue.empty())
    {
        continue;
    }

    //Ебливый компилятор кто тебе сука разрешал лезть в ячейки памяти которые записаны в очереди
    //СУКАААААААААААААААААААААААААААААААААААААААААААААААААААААААААААААААААААААААА
    //ЕБАНЫЙ В РОТ
    //ТРИ ЧАСА НА СМАРКУ ЧТОБЫ ПОНЯТЬ
    //ЧТО ПРОЦЕСС МОЖЕТ ПЕРЕЗАПИСЫВАТЬ ЯЧЕЙКИ ПО СВОЕЙ ХОТЕЛКЕ
    //ДАЖЕ ЕСЛИ ЯЧЕЙКА НЕ ПУСТАЯ
    //ЕБАТЬ

    //Сюда пришли если очередь не пуста, откусываем и начинаем обработку
    mutex.tryLock(-1);
    tempVector = packetQueue.front();
    packetQueue.pop();
    mutex.unlock();
    //передаем в локальные переменные чтобы не мучатьь себе голову на протяжении всего кода из-за структуры
    packetSize = tempVector.size();
    packetBuffer = (tUINT8*)malloc(packetSize);
    memcpy(packetBuffer,tempVector.data(),packetSize);
    packetCursor = packetBuffer;

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
    case ETPT_CLIENT_HELLO:
    {
        if(!HandleHelloPacket())
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

ReadingPacket:
    //Копируем размер чанка, если он у нас не существует
    if(chunkSize==0)
    {
        memcpy(&chunkSize,packetCursor,sizeof(tUINT32));
        chunkSize = GET_USER_HEADER_SIZE(chunkSize);
        std::cout<<chunkSize<<std::endl;
        //генерируем временный вектор для чанков, выходящих за рамки пакетов
        tempBuffer = (tINT8*)malloc(chunkSize);
        //Делаем ресайз вектора, присваиваем буфер вектору
        bufferVector.resize(chunkSize);
        chunkBuffer = bufferVector.data();
        //Присваеиваем курсор вектору
        p_chunkPointer = chunkBuffer;
    }

    //Мы переместили 50к байт, нам приходит еще 50к байт,
    //Это меньше, чем 110к байт, поэтому нам нужно добрать еще
    else if(bytesTransfered+bytesLeft<chunkSize)
    {
        //Копируем в конец вектора наш недостающий кусок
        memcpy(p_chunkPointer,packetCursor,bytesLeft);
        //Дополняем переданное количество байтов
        bytesTransfered+=bytesLeft;
        //Двигаем курсор по чанку внутри вектора
        p_chunkPointer+=bytesLeft;
        return false;
    }

    //Мы переместили 100кбайт, нам пришло еще 50кбайт
    //Это больше, чем 110к байт, поэтому заканчиваем буфер
    else if(bytesTransfered+bytesLeft>=chunkSize)
    {
        //Добиваем недозаполненный кусок в вектор
        memcpy(p_chunkPointer,packetCursor,chunkSize-bytesTransfered);
        //Передаем в очередь наш вектор
        chunkHandler.AppendChunksQueue(bufferVector);
        bufferVector.clear();//На всякий случай
        //Меняем значение bytesLeft на то, которое остается после транспортировки в буфер
        bytesLeft-=chunkSize-bytesTransfered;
        //Двигаем курсор пакета на начало нового буфера
        packetCursor+=chunkSize-bytesTransfered;
        //Обнуляем все, что нужно для чтения пакетиков
        chunkSize=0;
        bytesTransfered=0;
        goto ReadingPacket;
    }

    //-----------Обработка внутри одного пакета------------------//
    //Смотрим на пакеты внутри буфера
    if(chunkSize<=bytesLeft)
    {
        memcpy(p_chunkPointer,packetCursor,chunkSize);
        bytesLeft-=chunkSize; //Отнимаем прочитанный кусок от суммы пакета
        packetCursor+=chunkSize; //Перемещаем пакет по курсору, чтобы пропустить считанный чанк
        chunkHandler.AppendChunksQueue(bufferVector);
        bufferVector.clear();//На всякий случай
        chunkSize=0;
    }

    //Если у нас размер чанка больше, чем остаток внутри пакета
    else if(chunkSize>bytesLeft)
    {
        //А хули я все время не копировал прямо в вектор?
        //Потому что говно получается и вылезаю из-за этого скорее всего ошибки. Исправляем
        memcpy(p_chunkPointer,packetCursor,bytesLeft);
        //Скопировали в вектор, переместили курсор по нему
        p_chunkPointer+=bytesLeft;
        //bytesTransfered = сколько мы байтов перекинули в вектор,
        //по нему будем смотреть сколько нам не хватает для дозаполнения
        bytesTransfered = bytesLeft;
        bytesLeft = 0;
        return false;
    }

    //Если пакет прочитан не полностью - продолжаем его читать
    if(bytesLeft!=0)
    {
        chunkSize=0;
        goto ReadingPacket;
    }//Если пакет прочитан полностью - выходим из метода
    else
    {
        chunkSize=0;
        return true;
    }

}



tUINT32 PacketHandler::GetPacketType(sH_Packet_Header pckHdr)
{
    return (packetHeader.wBits &0xFu);
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
    // хз не помню почему такие ответы должны быть, если должны быть, значит зачем-то должны быть
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

    if (bytesOut == SOCKET_ERROR) {
        std::cout<<"Can't send DataReport responce, error: "<< WSAGetLastError()<<std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

bool PacketHandler::HandlePingPacket()
{
    counterPacketsFromServer++;
    // хз не помню почему такие ответы должны быть, если должны быть, значит зачем-то должны быть
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

    if (bytesOut == SOCKET_ERROR) {
        std::cout<<"Can't send DataReport responce, error: "<< WSAGetLastError()<<std::endl;
        WSACleanup();
        return false;
    }
    return true;
}

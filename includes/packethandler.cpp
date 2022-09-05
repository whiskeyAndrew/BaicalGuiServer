#include "packethandler.h"



void PacketHandler::run()
{
    //Туду: здесь должен быть обработчик пакетов. Нужно брать из очереди буфер и обрабатывать его так же, как это было в варианте с одним соединением
    //Берем из очереди буфер
    GetPacketFromQueue();
    //Обрабатываем
    PacketProcessing();
}

void PacketHandler::AppendQueue(char* bufferPointer, int bufferSize){
    //Дополняем очередь packetBuffer-а
    mutex.tryLock(-1);
    BufferData bufferData{bufferPointer,bufferSize};
    packetQueue.push(bufferData);
    mutex.unlock();
}

void PacketHandler::GetPacketFromQueue()
{
    //Ждем пока очередь чем-то заполнится, НА ВСЯКИЙ СЛУЧАЙ
    while(packetQueue.empty())
    {
        continue;
    }

    //Сюда пришли если очередь не пуста, откусываем и начинаем обработку
    mutex.tryLock(-1);
    dataFromQueue = packetQueue.front();
    packetQueue.pop();
    mutex.unlock();
    //передаем в локальные переменные чтобы не мучатьь себе голову на протяжении всего кода из-за структуры
    packetBuffer = dataFromQueue.packetPointer;
    packetCursor = packetBuffer;

    packetSize = dataFromQueue.sizeBuffer;

}

bool PacketHandler::PacketProcessing()
{
    //Читаем заголовок пакета
    memcpy(&packetHeader,packetCursor,sizeof(sH_Packet_Header));
    packetCursor+=sizeof(sH_Packet_Header);

    packetType = GetPacketType(packetHeader);
    lastPacketNumber = packetHeader.dwID;

}

int PacketHandler::GetPacketType(sH_Packet_Header pckHdr)
{
    return (packetHeader.wBits &0xFu);
}

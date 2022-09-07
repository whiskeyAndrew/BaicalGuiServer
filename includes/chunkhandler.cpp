#include "chunkhandler.h"

void ChunkHandler::run()
{

}

bool ChunkHandler::AppendChunksQueue(std::vector<tINT8> newVector)
{
    mutex.tryLock(-1);
    chunks.push(newVector);
    mutex.unlock();
    //std::cout<<chunks.size()<<std::endl;
}

ChunkHandler::ChunkHandler()
{

}

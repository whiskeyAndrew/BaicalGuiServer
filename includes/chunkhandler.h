#ifndef CHUNKHANDLER_H
#define CHUNKHANDLER_H

#include <QThread>
#include <queue>
#include <vector>
#include "GTypes.h"
#include <QMutex>
#include <iostream>

class ChunkHandler : public QThread
{
private:
    std::queue<std::vector<tINT8>> chunks;
    QMutex mutex;
    void run();
public:
    bool AppendChunksQueue(std::vector<tINT8> newVector);
    ChunkHandler();


};

#endif // CHUNKHANDLER_H

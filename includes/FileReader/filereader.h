#ifndef FILEREADER_H
#define FILEREADER_H
#include <QThread>
#include <iostream>
#include "../GTypes.h"
#include "../tracewindow.h"
#include <vector>
#include <QFile>

#pragma pack(push,2)


#pragma pack(pop)
class FileChunksHandler;

class FileReader:public QThread
{
    Q_OBJECT
private:
    QByteArray data;
    FileChunksHandler *fileChunksHandler;

    TraceWindow *traceWindow;
    QString fileName;
    tUINT64 fileSize;

    tUINT32 bufferSize;
    tINT8* fileBuffer;
    tINT8* bufferCursor;
    tINT8* bufferEnd;

    tINT8* chunkBuffer;
    tUINT32 chunkSize;

    std::vector<char> dataVector;
    bool ReadFileData();
    bool HandlingChunks();

public:
    FileReader();
    ~FileReader();
    void setTraceWindow(TraceWindow *newTraceWindow);
    void setFileName(QString newFileName);
    void run();
};

#endif // FILEREADER_H

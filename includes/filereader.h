#ifndef FILEREADER_H
#define FILEREADER_H
#include <QThread>
#include <iostream>
#include "GTypes.h"
#include "chunkhandler.h"
#include <vector>
#include <QFile>


class FileReader:public QThread
{
    Q_OBJECT
private:
    sP7File_Header fileHeader;

    ChunkHandler chunkHandler;

    QByteArray data;

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

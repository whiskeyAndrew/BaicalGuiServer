#ifndef FILEREADER_H
#define FILEREADER_H
#include <QThread>
#include <iostream>
#include "GTypes.h"
#include "chunkhandler.h"
#include <vector>
#include <QFile>
#pragma pack(push,2)
struct sP7File_Header
{
    tUINT64 qwMarker; //- 0x45D2AC71ECF32CA6ULL, 8 байт. Возможно, является маркером для байкал-сервера, чтобы по нему сервер распознавал, правильного ли формата он пытается открыть файл или нет.
    tUINT32 dwProcess_ID; //4 байта – ID процесса, берется из WINAPI GetCurrentProcessID
    tUINT32 dwProcess_Start_Time_Hi;//4байта - время создания файла, HI
    tUINT32 dwProcess_Start_Time_Lo; //время создания файла, LO
    char process_Name[512]; //
    char host_Name[512];
};
#pragma pack(pop)

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

#ifndef FILEREADER_H
#define FILEREADER_H
#include <QThread>
#include <iostream>
#include "../GTypes.h"
#include "../TraceWindow/tracewindow.h"
#include <vector>
#include <QFile>
#include "../chunkhandler.h"
//ради одного енама? Надо будет поправить
#include "../packethandler.h"
#include <QApplication>
#pragma pack(push,2)


#pragma pack(pop)
class FileChunksHandler;

class FileReader:public QThread
{
    Q_OBJECT
private:
    QByteArray data;


    TraceWindow* traceWindow;
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
    QFile* file;

public:
    ChunkHandler chunkHandler;
    FileReader(QString fileName, TraceWindow* newTraceWindow);
    ~FileReader();
    void run();
signals:
    void setSettingsDisabled(bool status);
};

#endif // FILEREADER_H

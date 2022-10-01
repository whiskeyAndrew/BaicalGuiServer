#ifndef TRACEBACKUPWRITER_H
#define TRACEBACKUPWRITER_H
#include "GTypes.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <iostream>

#define P7_DAMP_FILE_MARKER_V1                           (0x45D2AC71ECF32CA6ULL)
#define FILE_HEADER_SIZE (1044)
#pragma pack(push,2)

struct sP7File_Header{
    tUINT64 qwMarker = P7_DAMP_FILE_MARKER_V1; //- 0x45D2AC71ECF32CA6ULL, 8 байт. Возможно, является маркером для байкал-сервера, чтобы по нему сервер распознавал, правильного ли формата он пытается открыть файл или нет.
    tUINT32 dwProcess_ID; //4 байта – ID процесса, берется из WINAPI GetCurrentProcessID
    tUINT32 dwProcess_Start_Time_Hi;//4байта - время создания файла, HI
    tUINT32 dwProcess_Start_Time_Lo; //время создания файла, LO
    char process_Name[512] = {'t','e','m','p'};
    char host_Name[512] = {'t','e','m','p'};
};

#pragma pack(pop)

class TraceBackupWriter
{
private:
    sP7File_Header fileHeader;
    tINT8* data;
    QString fileName;
public:
    TraceBackupWriter();
    void setFileHeader(tUINT32 dwProcess_ID, tUINT32 dwProcess_Start_Time_Hi, tUINT32 dwProcess_Start_Time_Lo);
    void writeChunk(tINT8* chunk, tUINT32 size);
};

#endif // TRACEBACKUPWRITER_H

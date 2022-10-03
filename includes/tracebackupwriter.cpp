#include "tracebackupwriter.h"

void TraceBackupWriter::setFileHeader(tUINT32 dwProcess_ID, tUINT32 dwProcess_Start_Time_Hi, tUINT32 dwProcess_Start_Time_Lo)
{
    if(!QDir( "Backups").exists()){
            QDir().mkdir("Backups");
    }
    //Инициализация файла, в файл запишем его заголовок
    fileHeader.dwProcess_ID = dwProcess_ID;
    fileHeader.dwProcess_Start_Time_Hi = dwProcess_Start_Time_Hi;
    fileHeader.dwProcess_Start_Time_Lo = dwProcess_Start_Time_Lo;

    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("dd.MM.yyyy hh.mm.ss");

    fileName ="Backups/"+formattedTime+".p7d";

    QFile file(fileName);
    //временно, исправить
    file.open(QFile::WriteOnly);

    if(!file.open(QFile::WriteOnly))
    {
    std::cout<<"File opening error"<<std::endl;
    }

    data = (tINT8*)malloc(FILE_HEADER_SIZE);
    memcpy(data,&fileHeader,FILE_HEADER_SIZE);
    file.write(data,FILE_HEADER_SIZE);
    file.flush();
    //file.close();

    free(data);
}

void TraceBackupWriter::writeChunk(tINT8 *chunk, tUINT32 size)
{
    //пишем размер чанка и сам чанк
    QFile file(fileName);

    if(!file.open(QFile::WriteOnly | QIODevice::Append))
    {
    std::cout<<"File opening error"<<std::endl;
    }

    tUINT32 chunkSize = size+4;

    data = (tINT8*)malloc(chunkSize);
    memcpy(data,&chunkSize,sizeof(tUINT32));
    data+=sizeof(tUINT32);
    memcpy(data,chunk,size);
    data-=4;

    file.write(data,chunkSize);
    file.flush();
    free(data);
}


TraceBackupWriter::TraceBackupWriter()
{

}



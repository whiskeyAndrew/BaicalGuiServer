#include "tracebackupwriter.h"

void TraceBackupWriter::setFileHeader(tUINT32 dwProcess_ID, tUINT32 dwProcess_Start_Time_Hi, tUINT32 dwProcess_Start_Time_Lo)
{
    std::cout<<"Backup Writer: Opening backup file"<<std::endl;
    if(!QDir( "Backups").exists()){
            QDir().mkdir("Backups");
    }
    //Инициализация файла, в файл запишем его заголовок
    fileHeader.dwProcess_ID = dwProcess_ID;
    fileHeader.dwProcess_Start_Time_Hi = dwProcess_Start_Time_Hi;
    fileHeader.dwProcess_Start_Time_Lo = dwProcess_Start_Time_Lo;

    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("dd.MM.yyyy hh.mm.ss");

    fileName ="Backups/"+formattedTime+"_"+ QString::number(GetCurrentTime())+".p7d" ;

    file = new QFile(fileName);
    //временно, исправить

    if(!file->open(QFile::ReadWrite))
    {
    std::cout<<"File opening error"<<std::endl;
    }

    data = (tINT8*)malloc(FILE_HEADER_SIZE);
    memcpy(data,&fileHeader,FILE_HEADER_SIZE);
    file->write(data,FILE_HEADER_SIZE);
    file->flush();
    //file.close();

    free(data);
}

void TraceBackupWriter::writeChunk(tINT8 *chunk, tUINT32 size)
{
    //пишем размер чанка и сам чанк

    if(!file->isOpen()){
        std::cout<<"Backup file is not opened! Part of data will be erased"<<std::endl;
        return;
    }

    tUINT32 chunkSize = size+4;

    data = (tINT8*)malloc(chunkSize);
    memcpy(data,&chunkSize,sizeof(tUINT32));
    data+=sizeof(tUINT32);
    memcpy(data,chunk,size);
    data-=4;

    file->write(data,chunkSize);
    file->flush();
    free(data);
}

void TraceBackupWriter::closeFile(){
    std::cout<<"BackupWriter: closing backupFile"<<std::endl;
    file->close();
    delete file;
}
TraceBackupWriter::TraceBackupWriter()
{

}



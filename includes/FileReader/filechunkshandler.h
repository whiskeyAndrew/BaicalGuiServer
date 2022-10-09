#ifndef FILECHUNKSHANDLER_H
#define FILECHUNKSHANDLER_H
#include <QThread>
#include "../tracewindow.h"
#include "filereader.h"
#include <queue>
#include <QMutex>
#include "../chunkhandler.h"

class FileChunksHandler: public QThread
{
    Q_OBJECT

public:
    FileChunksHandler();
    void SetTraceWindow(TraceWindow *traceWindow);
    void AppendChunks(std::vector<tINT8>);

    void setFileEnded(bool newFileEnded);

private:
   std::queue<TraceToGUI> traceQueue;
   QMutex mutex;
   FileReader fileReader;
   TraceWindow *traceWindow;
   Trace trace;

   std::queue<std::vector<tINT8>> chunks;
   std::vector<tINT8 > chunkVector;

   tINT8* chunkBuffer;
   tINT8* chunkCursor;
   tINT8* chunkEnd;

   tUINT32 Ext_Raw;
   tUINT32 structSubtype;
   tUINT32 structSize;

   bool fileEnded = false;
   void run();
   void GetChunkFromQueue();
   bool ProcessChunk();
signals:
   void SendTraceFromFile(std::queue<TraceToGUI>);
   void SendTraceAsObject(Trace *trace);
};

#endif // FILECHUNKSHANDLER_H

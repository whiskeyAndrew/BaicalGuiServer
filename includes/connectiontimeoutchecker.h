#ifndef CONECTIONTIMEOUTCHECKER_H
#define CONECTIONTIMEOUTCHECKER_H

#include <QObject>
#include <QThread>
#include <QList>
#include "launcher.h"
#include "chunkhandler.h"

#define TIMEOUT_MSECS 3000

class ConnectionTimeoutChecker:public QThread
{
    Q_OBJECT
public:
    QList <ClientData>* windowsList;
    MainWindow *mainWindow;
    ConnectionTimeoutChecker(QList<ClientData>* windowsList,MainWindow *mainWindow){
        this->windowsList = windowsList;
        this->mainWindow = mainWindow;
        connect(this,&ConnectionTimeoutChecker::ClientDisconnected,this->mainWindow,&MainWindow::ChangeClientStatus);
    };

private:
    void run();
signals:
    void ClientDisconnected(sockaddr_in client);
};

#endif // CONECTIONTIMEOUTCHECKER_H

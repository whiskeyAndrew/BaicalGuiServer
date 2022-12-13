#ifndef CONECTIONTIMEOUTCHECKER_H
#define CONECTIONTIMEOUTCHECKER_H

#include <QObject>
#include <QThread>
#include <QList>
#include "launcher.h"

#define TIMEOUT_MSECS 3000

class ConnectionTimeoutChecker:public QThread
{
    Q_OBJECT
public:
    ConnectionTimeoutChecker(QList<ClientData>* newWindowsList,MainWindow* newMainWindow);

private:
    QList <ClientData>* windowsList;
    MainWindow* mainWindow;
    void run();
signals:
    void clientDisconnected(sockaddr_in client);
};

#endif // CONECTIONTIMEOUTCHECKER_H

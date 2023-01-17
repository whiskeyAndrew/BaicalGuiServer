#ifndef CONECTIONTIMEOUTCHECKER_H
#define CONECTIONTIMEOUTCHECKER_H

#include <QObject>
#include <QThread>
#include <QList>
#include "launcher.h"

#define TIMEOUT_MSECS 1000

struct ClientsAttemptToReconnect{
    sockaddr_in clientIp;
    PacketHandler* connectionThread;
    tUINT32 attempts;
};

class ConnectionTimeoutChecker:public QThread
{
    Q_OBJECT
public:
    ConnectionTimeoutChecker(MainWindow* newMainWindow);
    QWaitCondition waitCondition;
    void appendClientsMap(ClientData client);
    void removeClientAt(tUINT32 clientNumber);
private:
    QList<ClientsAttemptToReconnect> attemptsToReconect;
    tBOOL isAttemptsToReconnectChanged = false;
    MainWindow* mainWindow;
    void run();
signals:
    void clientStatusIsChanged(sockaddr_in client, tUINT32 status);
};

#endif // CONECTIONTIMEOUTCHECKER_H

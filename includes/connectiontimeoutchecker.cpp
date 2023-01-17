#include "connectiontimeoutchecker.h"
#define MAX_ATTEMPTS_TO_RECONNECT 5

ConnectionTimeoutChecker::ConnectionTimeoutChecker(MainWindow *newMainWindow){
    mainWindow = newMainWindow;
    connect(this,&ConnectionTimeoutChecker::clientStatusIsChanged,this->mainWindow,&MainWindow::changeClientStatus);
}

void ConnectionTimeoutChecker::appendClientsMap(ClientData client)
{
    attemptsToReconect.append({client.clientIp,client.connectionThread,0});
    isAttemptsToReconnectChanged = true;
}

void ConnectionTimeoutChecker::removeClientAt(tUINT32 clientNumber)
{
    ClientsAttemptToReconnect client = attemptsToReconect.at(clientNumber);

    //На случай если по каким-то причинам лаунчер попросил удалить раньше времени клиента
    //Надо убедиться что его статус изменился
    if(client.connectionThread->isInterruptionRequested()){
        std::cout<<"Connection lost from "<< ntohs(client.clientIp.sin_port)<<std::endl;
        emit clientStatusIsChanged(client.connectionThread->getClient(),OFFLINE);
        client.connectionThread->requestInterruption();
        client.connectionThread->waitCondition.wakeAll();
        DebugLogger::writeData(&"Connection lost from " [ ntohs(client.clientIp.sin_port)]);
    }

    attemptsToReconect.removeAt(clientNumber);
    isAttemptsToReconnectChanged = true;
}
void ConnectionTimeoutChecker::run()
{
    //Не идеальная система, так как она проверяет по одному соединению за раз. То есть если офнуть два соединения сразу, то она будет их по очереди разбирать
    //Можно будет доделать, если будет нужда
    DebugLogger::writeData("ConnectionTimeoutChecker:: launched!");

    while(!this->isInterruptionRequested()){
        for(int i =0;i<attemptsToReconect.size();i++){

            if(isAttemptsToReconnectChanged){
                std::cout<<"ConnectionTimeoutChecker: map changed, starting from 0" <<std::endl;
                isAttemptsToReconnectChanged = false;
                break;
            }

            ClientsAttemptToReconnect* client = &attemptsToReconect[i];

            //Это файл, лаунчер просто должен знать что в главном окне на этом месте есть ЧТО-ТО, чтобы нужные карты не сбивались со счета
            if(client->connectionThread==NULL){
                continue;
            }

            if(client->connectionThread->isFinished() || client->connectionThread->isInterruptionRequested()){
                continue;
            }

            if(client->attempts==MAX_ATTEMPTS_TO_RECONNECT){
                std::cout<<"Connection lost from "<< ntohs(client->clientIp.sin_port)<<std::endl;
                emit clientStatusIsChanged(client->connectionThread->getClient(),OFFLINE);
                client->connectionThread->requestInterruption();
                client->connectionThread->waitCondition.wakeAll();
                DebugLogger::writeData(&"Connection lost from " [ ntohs(client->clientIp.sin_port)]);
                continue;
            }

            if(client->connectionThread->getLastPacketTime()+TIMEOUT_MSECS<GetCurrentTime()){
                if(client->attempts==0){
                    emit clientStatusIsChanged(client->connectionThread->getClient(),UNKNOWN_CONNECTION_STATUS);
                }
                std::cout<<"Connection lost? Trying again, port: "<< ntohs(client->clientIp.sin_port) << " Attempt:" << client->attempts <<std::endl;
                client->attempts = client->attempts+1;
            }  else if(client->attempts!=0){
                client->attempts = 0;
                std::cout<<"Connection restored: "<< ntohs(client->clientIp.sin_port) <<std::endl;
                emit clientStatusIsChanged(client->connectionThread->getClient(),ONLINE);
            }

        }
        this->msleep(TIMEOUT_MSECS);
    }

    //Закрываем приложение
    if(this->isInterruptionRequested()){
        for(int i =0;i<attemptsToReconect.size();i++){
            ClientsAttemptToReconnect client = attemptsToReconect.at(i);
            client.connectionThread->requestInterruption();
            while(!client.connectionThread->isFinished()){
                client.connectionThread->waitCondition.wakeOne();
                client.connectionThread->wait(50);
            }
        }

        DebugLogger::writeData("ConnectionTimeoutChecker:: ending!");
        std::cout<<"------"<<"ConnectionTimeoutChecker is ending"<<"------"<<std::endl;
    }
}



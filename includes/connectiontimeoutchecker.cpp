#include "connectiontimeoutchecker.h"

ConnectionTimeoutChecker::ConnectionTimeoutChecker(QList<ClientData> *newWindowsList, MainWindow *newMainWindow){
    windowsList = newWindowsList;
    mainWindow = newMainWindow;
    connect(this,&ConnectionTimeoutChecker::clientStatusIsChanged,this->mainWindow,&MainWindow::changeClientStatus);
}

void ConnectionTimeoutChecker::run()
{
    while(true){

        for(int i=0;i<windowsList->size();i++){
            tUINT32 attempts = 0;
tryAgain:
            if(windowsList->at(i).connectionThread==NULL){
                continue;
            }
            if(windowsList->at(i).connectionThread->isFinished()){
                continue;
            }

            if(attempts==5){
                std::cout<<"Connection lost from "<< ntohs(windowsList->at(i).clientIp.sin_port)<<std::endl;
                emit clientStatusIsChanged(windowsList->at(i).connectionThread->getClient(),OFFLINE);
                windowsList->at(i).connectionThread->requestInterruption();
                windowsList->at(i).connectionThread->waitCondition.wakeAll();

                continue;
            }

            if(windowsList->at(i).connectionThread->getLastPacketTime()+TIMEOUT_MSECS<GetCurrentTime()){
                attempts++;
                std::cout<<"Connection lost? Trying again, port: "<< ntohs(windowsList->at(i).clientIp.sin_port) << " Attempt:" << attempts <<std::endl;
                if(attempts==1){
                    emit clientStatusIsChanged(windowsList->at(i).connectionThread->getClient(),UNKNOWN_CONNECTION_STATUS);
                }
                this->msleep(1000);
                goto tryAgain;
            }
            else if(attempts!=0){
                attempts = 0;
                std::cout<<"Connection restored: "<< ntohs(windowsList->at(i).clientIp.sin_port) <<std::endl;
                emit clientStatusIsChanged(windowsList->at(i).connectionThread->getClient(),ONLINE);
            }
        }

        //Закрываем приложение
        if(this->isInterruptionRequested()){
            for(int j = 0;j<windowsList->size();j++){
                windowsList->at(j).connectionThread->requestInterruption();
                while(!windowsList->at(j).connectionThread->isFinished()){
                    windowsList->at(j).connectionThread->waitCondition.wakeOne();
                    windowsList->at(j).connectionThread->wait(50);
                }

            }
            break;
        }
        this->msleep(TIMEOUT_MSECS);
    }

    std::cout<<"------"<<"ConnectionTimeoutChecker is ending"<<"------"<<std::endl;
}

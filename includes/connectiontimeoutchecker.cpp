#include "connectiontimeoutchecker.h"

void ConnectionTimeoutChecker::run(){
    while(true){

        for(int i=0;i<windowsList->size();i++){
<<<<<<< Updated upstream
            if(windowsList->at(i).connectionThread->getLastPacketTime()+TIMEOUT_MSECS<GetCurrentTime()){
                emit ClientDisconnected(windowsList->at(i).clientIp);
=======
            tUINT32 attempts = 0;
tryAgain:
            if(windowsList->at(i).connectionThread->isInterruptionRequested()){
                continue;
            }

            if(attempts==5){
                std::cout<<"Connection lost from "<< ntohs(windowsList->at(i).clientIp.sin_port)<<std::endl;
                emit ClientDisconnected(windowsList->at(i).connectionThread->client);
                windowsList->at(i).connectionThread->requestInterruption();
                continue;
            }

            if(windowsList->at(i).connectionThread->getLastPacketTime()+TIMEOUT_MSECS<GetCurrentTime()){
                attempts++;
                std::cout<<"Connection lost? Trying again, port: "<< ntohs(windowsList->at(i).clientIp.sin_port) << " Attempt:" << attempts <<std::endl;
                this->msleep(100);
                goto tryAgain;
>>>>>>> Stashed changes
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

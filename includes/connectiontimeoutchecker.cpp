#include "connectiontimeoutchecker.h"

void ConnectionTimeoutChecker::run(){
    while(true){
        for(int i=0;i<windowsList->size();i++){
            if(windowsList->at(i).connectionThread->getLastPacketTime()+TIMEOUT_MSECS<GetCurrentTime()){
                emit ClientDisconnected(windowsList->at(i).connectionThread->client);
            }
        }
        this->msleep(TIMEOUT_MSECS);
    }
}

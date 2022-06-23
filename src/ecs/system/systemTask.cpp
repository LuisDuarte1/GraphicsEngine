#include "systemTask.h"


SystemTask::SystemTask(System * nsystemToUse) : Task(), systemToUse(nsystemToUse){
    if(systemToUse->runSameThread) toRunInSameThread = true;
}


void SystemTask::tryMutate(){
    //TODO: maybe include support for mutations?
}

 void SystemTask::runTaskFunction(){
    systemToUse->updateSystem();
}
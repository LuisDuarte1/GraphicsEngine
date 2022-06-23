#ifndef SYSTEMTASK_ECS_H
#define SYSTEMTASK_ECS_H

#include <Taskus.h>
#include "system.h"

class SystemTask : public Taskus::Task{
    public:
        SystemTask(System * nsystemToUse);
        void tryMutate();
        void runTaskFunction();

        System * systemToUse;
};


#endif
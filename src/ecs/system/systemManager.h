#ifndef SYSTEMMANAGER_ECS_H
#define SYSTEMMANAGER_ECS_H

#include <vector>
#include <map>
#include <algorithm>
#include <Taskus.h>


#include "system.h"
#include "systemTask.h"
/*
The system manager should manage the TaskFlow of different systems. 
It should manage dependencies between components and make the systems fire events accordingly to those.

*/
struct Connection{
    size_t startDepth;
    size_t endDepth;
    Taskus::Task *  startTask;
    //the end Task is irrelevant, if there is a conflict we need only the start to solve it
};



class SystemManager{
    public:
        static SystemManager& getInstance(){
            static SystemManager instance;
            return instance;
        }
        //function to add systems that are dependent to each other
        void addSystem(std::vector<System *> dependenciesSystem, System * newSystem);
        void addSystem(System * newSystem);
        //singleton related function to not allow normal instaciation
        SystemManager(SystemManager&) = delete;
        void operator=(SystemManager const &) = delete;

        bool removeSystem(std::string systemName); //this will deallocate the memory that the system takes

        void updateAllSystems(Taskus::TaskPool& tPool);



    private:

        //TODO: maybe consider a "linked list aproach" with branching to allow for parallelism?
        std::vector<System*> systems;

        std::map<Taskus::Task *, int> depthMap;
        std::vector<Connection> connectionVector;



        Taskus::placeholderTask * entryTask = nullptr;
        Taskus::placeholderTask * endTask = nullptr;

        uint8_t timesNeededToOrder = 255;
        SystemManager();

};

#endif
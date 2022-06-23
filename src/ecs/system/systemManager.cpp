#include "systemManager.h"

Taskus::Task* findSystemTask(Taskus::Task * start, System * systemToFind){
    if(start->dependentTasks.size() == 0)return nullptr;
    Taskus::Task * taskFound = nullptr;
    for(Taskus::Task * dTask : start->dependentTasks){
        SystemTask * sysTask = dynamic_cast<SystemTask*>(dTask);
        if(sysTask == nullptr) continue;
        if(sysTask->systemToUse == systemToFind){
            taskFound = dTask;
            break;
        }
        Taskus::Task * ddTask = findSystemTask(dTask, systemToFind);
        if(ddTask == nullptr) continue;
        taskFound = ddTask;
        break;
    }
    return taskFound;
}



void findDepths(std::map<Taskus::Task *, int>& depthMap, Taskus::Task * startTask,int depth=0){
    //the depth is always the maximum value because a task could have multiple depths
    //but because it has to wait for the highest depth branch we assign the maximum depth
    auto found = depthMap.find(startTask);
    //if task doesn't exist in the map we add it with the current depth
    //if task exists we check if the stored depth is lower than the current depth
    //if it is we assign the new depth
    if(found == depthMap.end()) depthMap.insert({startTask, depth});
    else found->second = found->second < depth ? depth : found->second;
    if(startTask->dependentTasks.size() == 0) return;

    int newdepth = depth++;
    for(Taskus::Task * dTask : startTask->dependentTasks)
        findDepths(depthMap, dTask, newdepth);
}

void buildConnectionVector(std::vector<Connection>& connectionVector, 
const std::map<Taskus::Task *, int>& depthMap, Taskus::Task* rootTask){
    if(rootTask->dependentTasks.size() == 0) return;
    for(Taskus::Task* dTask: rootTask->dependentTasks){
        Connection newCon;
        //with the depthMap becomes much easy to find out the max depth of each connection
        //and build the connection vector
        newCon.startDepth = depthMap.find(rootTask)->second;
        newCon.endDepth = depthMap.find(dTask)->second;
        newCon.startTask = rootTask;
        buildConnectionVector(connectionVector, depthMap, dTask);
    }        
}


std::vector<Taskus::Task*> findConflicts(Taskus::Task * firstTask, System * newSystem, 
std::vector<Connection>& connectionVector, std::map<Taskus::Task *, int>& depthMap){
    std::vector<Taskus::Task*> conflicts;
    //now we need to get the root and check of connections in the whole system
    Taskus::Task * root = nullptr;
    while (true)
    {
        if(firstTask->dependenciesTasks.size() == 0){
            root = firstTask;
            break;
        }
        //it's irrelevant which one we choose, they must always get to the root
        if(root == nullptr) root = firstTask->dependenciesTasks[0]; 
        if(root->dependenciesTasks.size() == 0) break;
        root = root->dependenciesTasks[0];
    }
    //now that we find the root, we check for connections, we only care for the ones that start 
    //before firstTask and finish after firstTask
    // we also care for the ones that start in firstTask (this ones are more easy to implement)
    depthMap.clear();
    connectionVector.clear();
    findDepths(depthMap, root);
    buildConnectionVector(connectionVector, depthMap, root);
    //we can compare connections easily by checking the "depths" of the start Task and the endTasks 
    //to the firstTask 
    //a conflict when the depth of the desired system is between the start and the end of a conneciton
    int thisTaskDepth = depthMap.find(firstTask)->second + 1;
    for(Connection conn : connectionVector){
        if(conn.startDepth <= thisTaskDepth && conn.endDepth > thisTaskDepth){
            //if it is between there might be a conflict but we need to check if there is
            //one equal component that is used between the two
            SystemTask * sysTask = dynamic_cast<SystemTask*>(conn.startTask);
            if(sysTask == nullptr) continue; //this probably means that this is the root and is not relevant?
            if(std::find_first_of(newSystem->componentsUsed.begin(), newSystem->componentsUsed.end(),
                sysTask->systemToUse->componentsUsed.begin(), sysTask->systemToUse->componentsUsed.end())
                != newSystem->componentsUsed.end()){
                    conflicts.push_back(sysTask);
            }
        }
    }
    

    return conflicts;

}


SystemManager::SystemManager(){
    entryTask = new Taskus::placeholderTask();
    endTask = new Taskus::placeholderTask();

}

bool SystemManager::removeSystem(std::string systemName){
    size_t toRemove = -1;
    for(size_t i = 0; i < systems.size(); i++){
        if(systems[i]->name == systemName){
            toRemove = i;
            break;
        }
    }
    if(toRemove == -1) return false;
    //remove from TaskFlow
    systems.erase(systems.begin() + toRemove);
    return true;
}

void SystemManager::updateAllSystems(Taskus::TaskPool& tPool){
    /*for(System * sys : systems){
        sys->update();
        sys->times_ran++;
        if(sys->times_ran == timesNeededToOrder){
            sys->order();
        }
    }
    
    almost the same logic as the code above but multithreaded when possible*/ 
    
    //we maybe don't use the isRepeatable flag to make it easier to add new systems at runtime? 
    //so we implement this as a loop 
    tPool.addTask(entryTask);
    endTask->waitToFinish();
}



void SystemManager::addSystem(System * newSystem){
    //adds the system at the begining of the Task flow
    //check if there are duplicates
    for(System * sys : systems){
        if(sys->name == newSystem->name) throw std::runtime_error("Tried to add an already existing system");
    }
    systems.push_back(newSystem);
    SystemTask * newSystemTask = new SystemTask(newSystem);
    if(entryTask->dependentTasks.size() == 0){
        (*entryTask) += newSystemTask;
        (*newSystemTask) += endTask;
    } else{
        //we need to check for dependencies on the components
        //instead of checking for conflicts in the nodes we check for conflicts in the connections because it's a better way to predict it
        //a system can run for the whole system but it would only be checked in the position that start, which might cause conflicts
        std::vector<Taskus::Task*> found_conflict = findConflicts(entryTask, newSystem, connectionVector, depthMap);


        //add the task after the entryTask above other tasks
        (*entryTask) += newSystemTask;
        if(found_conflict.size() == 0){
            return;
        }
        else{
            for(Taskus::Task * cTasks : found_conflict){
                (*cTasks) += newSystemTask;
            }
        }
    }
}

void SystemManager::addSystem(std::vector<System *> dependenciesSystem, System * newSystem){
    //adds the system after another one, if they are independent they can be run at the same time

    //check if there are duplicates
    for(System * sys : systems){
        if(sys->name == newSystem->name) throw std::runtime_error("Tried to add an already existing system");
    }
    systems.push_back(newSystem);
    //first we find the dependencySystem in the taskflow
    std::vector<Taskus::Task*> totalConflicts;
    std::vector<Taskus::Task*> dependencyTasks;
    for(System * dependencySystem: dependenciesSystem){
        Taskus::Task * systemTask = findSystemTask(entryTask, dependencySystem);
        if(systemTask == nullptr) {
            assert(false);
            return;
        }
        dependencyTasks.push_back(systemTask);
        std::vector<Taskus::Task*> thisConflicts = findConflicts(systemTask, newSystem, 
            connectionVector, depthMap);
        for(Taskus::Task * con : thisConflicts){
            if(std::find(totalConflicts.begin(), totalConflicts.end(), con) != totalConflicts.end()){
                totalConflicts.push_back(con);
            }
        }
    }
    SystemTask * newsysTask = new SystemTask(newSystem);
    for(Taskus::Task* dTask: dependencyTasks){
        *(dTask) += newsysTask;
    }
    for(Taskus::Task* cTask: totalConflicts){
        *(cTask) += newsysTask;
    }
    
}
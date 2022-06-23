#ifndef EVENTMANAGER_ECS_H
#define EVENTMANAGER_ECS_H

#include <unordered_map>
#include <typeinfo>
#include <mutex>

#include "event.h"

class EventManager{
    public:
        static EventManager& getInstance(){
            static EventManager instance;
            return instance;
        }
        //singleton related function to not allow normal instaciation
        EventManager(EventManager&) = delete;
        void operator=(EventManager const &) = delete;
        

        template<typename T> bool registerEventType(){
            std::lock_guard<std::mutex> lock(subscribersMutex);
            if(subscribers.find(typeid(T)) != subscribers.end()) return false; //false for already registred
            subscribers.insert({typeid(T), {}});
            return true;
        }

        template<typename T> bool subscribeToEvent(ISubscriber * subscriber){
            std::lock_guard<std::mutex> lock(subscribersMutex);
            auto found = subscribers.find(typeid(T));
            if(found == subscribers.end()) return false;
            bool exists = false;
            for(ISubscriber * i : (*found).second){
                if(i == subscriber){
                    exists = true;
                    break;
                }
            }
            if(!exists) return false;
            (*found).second.push_back(subscriber);
            return true;
        }

        /* fireEventWhenSubscriberRuns is used when the worst scenario happens. 
           The event can't be run immediatly because the destination Subscribers/Systems, have a dependency
           between other systems that are being run at the moment in parallel. This is not a optimal scenario
           but it can be done this. 

           Ideally, we want to run the function immediatly to avoid overhead using mutexes and lists.
        */

        template<typename T> void fireEventWhenSubscriberRuns(T * event){
            std::lock_guard<std::mutex> lock(subscribersMutex);
            auto found = subscribers.find(typeid(T));
            if(found == subscribers.end()) return;
            for(ISubscriber * i : (*found).second){
                i->fireEvent(event);
            }
        }

        template<typename T> void fireEventImmediatly(T * event){
            auto found = subscribers.find(typeid(T));
            if(found == subscribers.end()) return;
            for(ISubscriber * i, (*found).second){
                i->updateFunc(event);
            }
        }
        
    private:
        EventManager(){}
        
        std::unordered_map<type_info, std::vector<ISubscriber*>> subscribers;
        std::mutex subscribersMutex;

};

#endif
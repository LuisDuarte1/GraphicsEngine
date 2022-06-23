#ifndef EVENT_ECS_H
#define EVENT_ECS_H

#include <list>
#include <mutex>

//this event can be overriden to store different types of events 
//and use dynamic_cast (or a faster alternative with it like kcl_dynamic_cast)
class Event{
    public:
        virtual ~Event() = 0;
};


//ISubscriber should be thread safe, which mean it will update the events in the systems sequentially
//and on the next frame (if the system has already updated). 
class ISubscriber{
    public:
        virtual void updateFunc(Event * e) = 0; //should use dynamic_cast
        void update();
        void fireEvent(Event * e);

    private:
        std::list<Event * > eventsToFire;
        std::mutex eventsToFireMutex;


};




#endif
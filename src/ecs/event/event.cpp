#include "event.h"


void ISubscriber::fireEvent(Event * e){
    eventsToFireMutex.lock();
    eventsToFire.push_back(e);
    eventsToFireMutex.unlock();
}


void ISubscriber::update(){
    eventsToFireMutex.lock();
    for(Event * e: eventsToFire) updateFunc(e);
    eventsToFireMutex.unlock();
}
#ifndef SYSTEM_ECS_H
#define SYSTEM_ECS_H

#include <iostream>
#include <vector>
#include <typeindex>

#include "../component/componentSystem.h"
#include "../event/event.h"

/*
A system will alter the the components data in chunks, allowing for more cache locality of data and make
memory fetching a lot more faster. (for instance normal memory has a latency of ~250 cycles and L1 cache
has a latency of 3 cycles).
*/

//abstract class for a system
//a system is always a subsciber because it receives events from other systems
class System: public ISubscriber{
    public:
        System(std::string nname, bool nrunSameThread) : name(nname), runSameThread(nrunSameThread){}

        void updateSystem();

        //inherited from ISubscriber just here for readability
        //this is to process Events
        virtual void updateFunc(Event * e) = 0;

        //updates the system 
        virtual void updateSystemFunc() = 0; 
        //this will run periodically the order function to make cache locality even better
        //by sorting the component list by what the system needs more
        virtual void order() = 0;
        bool active = true;

        bool runSameThread;

        //this must be between 0-255 which mean the SystemManager must run the order function every 255 frames (worst case)
        //that the system runs (at 60 FPS means that it runs every 4.25 seconds, in the worst case).
        uint8_t times_ran = 0; 

        std::string name;

        virtual ~System(){}
        
        //this vector indicates what components are used to the system manager optimize the best
        //way to run this system (if the system has no component dependency to the last system,
        //they can be run in parallel).
        //in the constructor of every system we should specify by hand every type used
        std::vector<std::type_index> componentsUsed; 
};


#endif
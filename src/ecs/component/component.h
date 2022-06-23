#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>


/* The component class holds all the data. if T is a struct, it will most probably contain a few member 
    variables. Even though we have improved cache locality this is probably not the most optimal way to do
    it. Let's say that the a cache line holds 64 bytes. If he have a struct with 4 32-bit ints
    every struct has 16 bytes. So every cache line only can hold 4 components at the time. 

    Let's say that we seperate the struct into 4 vectors with a member variable each.
    Every cache line holds 16 elements. But we have to read 4 from memory 4 times. But it's probably
    still more efficient that the above approach.

    The latter maybe can be implemented in c++ with macros, not very sure how i'm going to do it yet.
    Maybe the best way is to use class template specification and a macro that writes the struct 
    and another that stores a class in SoA form.
*/


//base for non macro components
template<typename T>
class IComponent{
    public:
        void addNewComponent(){components.resize(components.size()+1);}
        std::vector<T> components;
        std::vector<uint32_t> entityIDs;
};


#define COMPONENT_CLASS_START(name) struct name{}; template<> class IComponent<name>{ public: \
    std::vector<uint32_t> entityIDs;

#define COMPONENT_FIELD(T, field_name) std::vector<T> field_name;

#define COMPONENT_CLASS_END() };

#endif
#ifndef COMPONENT_SYSTEM_H
#define COMPONENT_SYSTEM_H

#include "component.h"

#include <any>
#include <vector>


//component system is a singleton class
class ComponentSystem{

    public:
        //related to singleton design pattern, don't allow for = operator or copy constructor
        ComponentSystem(const ComponentSystem&) = delete;
        void operator=(ComponentSystem const&) = delete;
        static ComponentSystem & getInstance(){
            static ComponentSystem instance;
            return instance;
        }

        template<class T> bool registerComponentType(){
            bool found = false;
            for(auto i : components){
                if(i.type() == typeid(IComponent<T>*)){
                    found = true;
                    break;
                }
            }
            if(found){
                return false; //this means that a IComponent already exists
            }
            IComponent<T>* newICOMP = new IComponent<T>();
            components.push_back(newICOMP);
            return true;
        }

        template<class T> size_t getComponent(uint32_t entityID){
            for(auto i: components){
                if(i.type() == typeid(IComponent<T>*)){
                    IComponent<T> * icomp = std::any_cast<IComponent<T>*>(i);
                    for(size_t i = 0; i < icomp->entityIDs.size(); i++){
                        if(entityID == icomp->entityIDs[i]){
                            return i;
                        }
                    }
                }
            }

            return size_t(int(-1));
        }

        template<class T> bool addComponent(uint32_t entityID){
            for(auto i: components){
                if(i.type() == typeid(IComponent<T>*)){
                    T r = {};
                    IComponent<T> * icomp = std::any_cast<IComponent<T>*>(i);
                    //find if there is already a component
                    for(uint32_t i : icomp->entityIDs) if(i == entityID) return false;
                    icomp->addNewComponent();
                    //TODO: function for adding new entityID to component
                    icomp->entityIDs.push_back(entityID); 
                    return true;                    
                }
            }
            return false;
        }

        template<class T> IComponent<T> * getComponentList(){
            for(auto i: components){
                if(i.type() == typeid(IComponent<T>*)){
                    return std::any_cast<IComponent<T>*>(i);
                }
            }
            return nullptr;
        }

        template<class T> bool checkIfComponentIsRegistred(){
            for(auto i: components){
                if(i.type() == typeid(IComponent<T>*)){
                    return true;
                }
            }
            return false;
        }



    private:
        ComponentSystem(){}; //make constructor private to make a singleton design pattern
        std::vector<std::any> components;

};

#endif

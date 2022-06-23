#ifndef WIN32_WINDOW_TASK_H
#define WIN32_WINDOW_TASK_H

#include <Taskus.h>
#include "win32window.h"
#include "../../../../ecs/system/system.h"

class Win32WindowSystem : public System{
    public:
        Win32WindowSystem(std::string name, int width, int height) : System("win32WindowSystem", true), name(name), width(width), height(height){
        }
        void updateSystemFunc();

        void updateFunc(Event * e){}

        void order(){}
    private:
        std::string name;
        int width;
        int height;
};


#endif
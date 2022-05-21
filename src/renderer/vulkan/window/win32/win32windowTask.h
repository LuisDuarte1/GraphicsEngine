#ifndef WIN32_WINDOW_TASK_H
#define WIN32_WINDOW_TASK_H

#include <Taskus.h>
#include "win32window.h"

class Win32WindowTask : public Taskus::Task{
    public:
        Win32WindowTask(std::string name, int width, int height) : Task(), name(name), width(width), height(height){
            toRunInSameThread = true; //win32 api requires for a window message to run always in the same thread
        }
        void tryMutate(){}

        void runTaskFunction();
    private:
        std::string name;
        int width;
        int height;
};


#endif
#ifdef WIN32
#include <Windows.h>
#include "win32windowTask.h"


void Win32WindowTask::runTaskFunction(){
    if(win32Instance == 0){
        win32Instance = new Win32Window(name, width, height);
        win32Instance->createWindow();
        return;
    }
    //process message
    
}

#endif

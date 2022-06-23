#ifdef WIN32
#include <Windows.h>
#include "win32windowSystem.h"


void Win32WindowSystem::updateSystemFunc(){
    if(win32Instance == 0){
        win32Instance = new Win32Window(name, width, height);
        win32Instance->createWindow();
        return;
    }
    //TODO: process window and thread messages
    MSG msg;
    if(PeekMessage(&msg, NULL, 0, 0, 1)){
        TranslateMessage(&msg); 
        DispatchMessage(&msg);
    }
    
    
}

#endif

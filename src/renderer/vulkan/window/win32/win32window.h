#ifndef RENDER_WIN32_WINDOW_H
#define RENDER_WIN32_WINDOW_H

#define VK_USE_PLATFORM_WIN32_KHR

#include "../baseWindow.h"

#ifndef _WINDEF_
#include "win32basetypes.h"
#endif

LRESULT CALLBACK winproc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

class Win32Window; //fw decl

//bad singleton design, this only allows for one window but winproc can't be static in class 
//so this is a workaround
inline Win32Window * win32Instance = 0;

//A Task that handles the win32window should always run on the same thread
class Win32Window : public Window{
    public:
        Win32Window(std::string windowName, size_t width, size_t height);
        ~Win32Window();
        void createWindow();
        std::vector<InputMessage> getInput();
        void createSurface(VkInstance instance);
        void processInput(LPARAM lParam, bool keyDown);


    private: 
        WNDCLASS wc = {};
        HWND windowHWND;

};



#endif
#include <Windows.h>
#include "win32window.h"

const char g_szClassName[] = "vulkanwindowclass";


void Win32Window::processInput(LPARAM lParam, bool keyDown){
    char inputchar;
    int r = GetKeyNameText(lParam, &inputchar, 1); //every caracter should be one bit only? 
    //TODO: support other keyboard layouts by using the wide version of windows.h
    if(r == 0){
        std::cout << "Couldn't get the key name.\n";
        assert(false);
    }
    InputMessage m;
    m.keycode = inputchar;
    m.keyDown = keyDown;
    queuedInputsMutex.lock();
    queuedInputs.push_back(m);
    queuedInputsMutex.unlock();
}


LRESULT CALLBACK winproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
        case WM_KEYDOWN:
            win32Instance->processInput(lParam, true);
            break;
        case WM_KEYUP:
            win32Instance->processInput(lParam, false);
            break;
        case WM_MOUSEMOVE:
            //TODO: get mouse input
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd); 
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd,msg,wParam,lParam);

    }
    return 0;
}




Win32Window::Win32Window(std::string windowName, size_t width, size_t height) : Window(windowName,width,height){
    requiredExtensions =  {"VK_KHR_surface", "VK_KHR_win32_surface"};
    //don't allow for more than one win32 window
    if(win32Instance != 0) assert(false);
    win32Instance = this;
}


void Win32Window::createWindow(){
    wc.lpfnWndProc = reinterpret_cast<WNDPROC>(&winproc);
    wc.lpszClassName = g_szClassName;
    wc.hInstance = GetModuleHandle(nullptr);

    RegisterClass(&wc);
    windowHWND=CreateWindowEx(0,
                        g_szClassName,
                        windowName.c_str(),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,CW_USEDEFAULT,
                        width, height,
                        NULL,
                        NULL,
                        GetModuleHandle(nullptr),
                        NULL);
    if(windowHWND == NULL){
        DWORD error = GetLastError();
        void * msgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
        | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &msgBuf, 0, NULL);
        std::string err;
        char * it = (char *) msgBuf;
        while((*it) != 0){
            err += *it;
            it++;
        }
        std::cerr << "Win32WindowError: " << err << "\n";
        LocalFree(msgBuf);
        throw std::runtime_error("Couldn't create win32 window.");
    }

    ShowWindow(windowHWND, 1);
}


std::vector<InputMessage> Win32Window::getInput(){
    return {};
}

void Win32Window::createSurface(VkInstance instance){
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hinstance = GetModuleHandle(NULL);
    createInfo.hwnd = windowHWND;
    VkResult r = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &windowSurface);
    if(r != VK_SUCCESS) throw std::runtime_error("Couldn't create window win32 surface.");
    
}


Win32Window::~Win32Window(){

}
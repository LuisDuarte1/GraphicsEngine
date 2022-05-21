#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <iostream>
#include <vector>
#include <mutex>
#include "../../../input/input.h"
#include <vulkan/vulkan.h>


//window abstract class for allowing to the renderer inteface with it in multiple OSes
class Window{
    public:
        Window(std::string windowName, size_t width, size_t height) : windowName(windowName), width(width), height(height){}

        virtual void createWindow() = 0;
        virtual std::vector<InputMessage> getInput() = 0;
        virtual void createSurface(VkInstance instance) = 0;

        //get required Vulkan Extensions that the os might need for a window
        std::vector<const char *> getRequiredExtensions(){
            return requiredExtensions;
        }

        inline VkSurfaceKHR getSurface(){return windowSurface;};
        
    protected:
        std::vector<InputMessage> queuedInputs;
        std::mutex queuedInputsMutex;
        std::string windowName;
        bool createdWindow = false;
        VkSurfaceKHR windowSurface;
        std::vector<const char*> requiredExtensions;
        size_t width;
        size_t height;
};


#endif

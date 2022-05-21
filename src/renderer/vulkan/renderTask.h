#ifndef RENDERTASKVULKAN_HPP
#define RENDERTASKVULLAN_HPP

#include <iostream>
#include <Taskus.h>

#include "vulkanrenderer.h"


class RenderTask : public Taskus::Task{
    public:
        RenderTask(VulkanRenderer * renderer);

        void tryMutate();

        void runTaskFunction();
    
    private:
        VulkanRenderer * renderer;
};

#endif
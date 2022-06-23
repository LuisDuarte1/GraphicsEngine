#ifndef RENDERTASKVULKAN_HPP
#define RENDERTASKVULLAN_HPP

#include <iostream>
#include <Taskus.h>

#include "vulkanrenderer.h"
#include "../../ecs/system/system.h"
#include "renderObjectComponent.h"

class VulkanRenderSystem : public System{
    public:
        VulkanRenderSystem(VulkanRenderer * renderer) : System("Vulkan Render System", false),renderer(renderer){
            componentsUsed.emplace_back(typeid(RenderObjectComponent));
        };

        
        void updateSystemFunc();

        void updateFunc(Event * e){} //TODO: process needed events

        void order(){}
    
    private:
        VulkanRenderer * renderer;
};

#endif
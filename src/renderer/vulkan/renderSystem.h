#ifndef RENDERTASKVULKAN_HPP
#define RENDERTASKVULLAN_HPP

#include <iostream>
#include <Taskus.h>

#include "vulkanrenderer.h"
#include "../../ecs/system/system.h"
#include "../../utils/objreader.h"
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

        size_t lastRenderComponentSize = 0;

        void InitializeObject(VmaAllocator allocator, int i, IComponent<RenderObjectComponent>& componentRender);

        VulkanRenderer * renderer;
};

#endif
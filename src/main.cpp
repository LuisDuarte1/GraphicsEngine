#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>       /* time */
#include <random>
#include <functional>
#include <thread>
#include <Taskus.h>

#include "ecs/component/componentSystem.h"
#include "ecs/system/systemManager.h"

#include "renderer/vulkan/vulkanrenderer.h"
#include "renderer/vulkan/vulkan-pipeline/vulkan-pipeline.h"
#include "renderer/vulkan/window/baseWindow.h"
#include "renderer/vulkan/renderSystem.h"
#include "renderer/vulkan/renderObjectComponent.h"


#include "utils/objreader.h"
#include "object/object.h"
//#include "utils/objreader.h"
//#include "gamelogic/gamelogic.h"

#ifdef WIN32
    #include "renderer/vulkan/window/win32/win32windowSystem.h"
#endif


int main()
{
    ComponentSystem & tcomp = ComponentSystem::getInstance();
    SystemManager & sysMng = SystemManager::getInstance();
    if(!tcomp.registerComponentType<RenderObjectComponent>()) abort();
    Taskus::TaskPool tPool;
    tPool.start();
    VulkanRenderer* renderergl = nullptr;
    #ifdef WIN32
        Win32WindowSystem * windowSystem = new Win32WindowSystem("Vulkan Engine", 1024, 794);
        sysMng.addSystem(windowSystem);
        sysMng.updateAllSystems(tPool);
        renderergl = new VulkanRenderer(win32Instance);
    #endif

    #ifdef UNIX
        return 1;
    #endif
    VulkanRenderSystem * renderSystem = new VulkanRenderSystem(renderergl);
    renderergl->InitVulkan();

    Camera *camera = new Camera(glm::vec3(1.5f,-0.75f,6), glm::vec3(0,0,0));
    renderergl->SelectCurrentCamera(camera);

    VulkanPipeline simple_shaders;
    simple_shaders.loadShaders("simplevertshader.vert.spv", "simplefragshader.frag.spv", renderergl->GetCurrentDevice());
    SimpleUniformBuffer ubo;
    ubo.proj_matrix = glm::mat4(0.5);
    simple_shaders.loadUniformBuffer(&ubo, SIMPLE_TYPE, true);
    

    
    
    renderergl->AddPiplineToList(&simple_shaders);
    
    
    std::vector<float> color;

    std::vector<float> pos_sphere = ReadObjFile("sphere.obj");
    WorldObject Mati;
    srand(time(NULL));
    color = {};
    for(int i = 0; i<(pos_sphere.size()/5) * 3; i++){
        
        float r = float((rand() % 10 + 1)) / 10.0f; 
        float g = float((rand() % 10 + 1)) / 10.0f; 
        float b = float((rand() % 10 + 1)) / 10.0f; 
        color.push_back(r);
        color.push_back(g);
        color.push_back(b);
    }
    Mati.LoadObject(pos_sphere, color);
    Mati.ChangeWorldPosition(glm::vec3(1,-1.5,0));
    Mati.setAlbedoTexture(renderergl->getTextureManager()->AddTexture("SphereRamos.png", ALBEDO_TEXTURE));

    
    Mati.SendToGPU(renderergl->allocator, renderergl);
    Mati.pipeline_id = simple_shaders.pipeline_id;

    renderergl->AddObjectToRender(&Mati);

    

    renderergl->createCommandBuffers();
    

    

    //std::thread game_thread(&GameLogic::Init, gameloop);
    sysMng.addSystem(renderSystem);
    while(true){
        sysMng.updateAllSystems(tPool);
    }
    renderergl->cleanup();
    tPool.stop();
    //game_thread.join();

    return 0;
}


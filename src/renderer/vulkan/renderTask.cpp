#include "renderTask.h"


RenderTask::RenderTask(VulkanRenderer * renderer) : renderer(renderer){
    isRepeatable = true;
}

void RenderTask::tryMutate(){

}


void RenderTask::runTaskFunction(){
    //TODO: poll window input
    renderer->drawFrame();
}
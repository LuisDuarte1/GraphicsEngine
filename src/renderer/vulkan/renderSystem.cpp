#include "renderSystem.h"


template<typename T> void moveIntoIndex(std::vector<T> & vector, size_t indexStart, size_t indexFinal){

    T valueStart = vector[indexStart];

    T old;
    for(size_t i = indexFinal; i < vector.size()-1; i++){
        old = vector[i+1];        
        if(i == indexFinal) vector[i+1] = vector[i];
        else vector[i+1] = old;
    }
    vector[indexFinal] = valueStart;


}

void VulkanRenderSystem::updateSystemFunc(){
    //find uninitialized objects before rendering
    IComponent<RenderObjectComponent> * iComp = ComponentSystem::getInstance().getComponentList<RenderObjectComponent>();
    if(iComp->entityIDs.size() > lastRenderComponentSize){
        //initializing to do
        long diff = iComp->entityIDs.size() - lastRenderComponentSize;
        if(diff > 0){
            for(long i = 0; i < diff; i++){
                InitializeObject(renderer->allocator, i, *iComp);
            }
        }
    }
    lastRenderComponentSize = iComp->entityIDs.size();
    renderer->render(iComp);
}

void VulkanRenderSystem::InitializeObject(VmaAllocator allocator, int i, IComponent<RenderObjectComponent>& componentRender){
    if(componentRender.initialized[i]) return;
    //this will send to the gpu the list with vertex structs
    std::cout<<"Initializing object on the render system\n";
    //first we will send the vertex data to a buffer on the cpu...
    VkBufferCreateInfo bufferinfo = {};
    bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferinfo.size = componentRender.vertex_data[i].size() * sizeof(Vertex); //data in bytes
    bufferinfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;


    VmaAllocationCreateInfo vmamallocInfo = {};
    vmamallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VkBuffer stagingBuffer;
    VmaAllocation staging_alloc;

    if(vmaCreateBuffer(allocator, &bufferinfo, &vmamallocInfo, &stagingBuffer, &staging_alloc, nullptr) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create vertex buffer on the CPU...");
    }
    //copy memory into the cpu
    void *data;
    vmaMapMemory(allocator, staging_alloc, &data);
    memcpy(data, componentRender.vertex_data[i].data(), (componentRender.vertex_data[i].size()) * sizeof(Vertex));
    vmaUnmapMemory(allocator, staging_alloc);

    
    

    //now we create the buffer on the gpu
    VkBufferCreateInfo vertex_buffer_info = {};
    vertex_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertex_buffer_info.size =  componentRender.vertex_data[i].size() * sizeof(Vertex); //data in bytes
    vertex_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;


    vmamallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;


    if(vmaCreateBuffer(allocator, &vertex_buffer_info, &vmamallocInfo, &componentRender.vertex_buffer[i]
        , &componentRender.vertex_alloc[i], nullptr) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create vertex buffer on the CPU...");
    }

    renderer->immediateSubmitTransfer(stagingBuffer, componentRender.vertex_buffer[i],  componentRender.vertex_data[i].size() * sizeof(Vertex));

    
    //index buffer



    VkBufferCreateInfo indexBufferStagingInfo = {};
    indexBufferStagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexBufferStagingInfo.size = componentRender.index_data[i].size() * sizeof(uint16_t); //data in bytes
    indexBufferStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    vmamallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VkBuffer indexStagingBuffer;
    VmaAllocation indexstagingAlloc;
    if(vmaCreateBuffer(allocator, &indexBufferStagingInfo, &vmamallocInfo, &indexStagingBuffer, &indexstagingAlloc, nullptr) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create vertex buffer on the CPU...");
    }
    //copy memory into the cpu
    void *ddata;
    vmaMapMemory(allocator, indexstagingAlloc, &ddata);
    memcpy(ddata, componentRender.index_data[i].data(), componentRender.index_data[i].size() * sizeof(uint16_t));
    vmaUnmapMemory(allocator, indexstagingAlloc);

    //now we create the buffer on the gpu
    VkBufferCreateInfo indexBufferInfo = {};
    indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexBufferInfo.size = componentRender.index_data[i].size() * sizeof(uint16_t); //data in bytes
    indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;


    vmamallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;


    if(vmaCreateBuffer(allocator, &indexBufferInfo, &vmamallocInfo, &componentRender.index_buffer[i], 
        &componentRender.index_alloc[i], nullptr) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create vertex buffer on the CPU...");
    }

    renderer->immediateSubmitTransfer(indexStagingBuffer, componentRender.index_buffer[i],  
        componentRender.index_data[i].size() * sizeof(uint16_t));

    //we dont need the staging buffers anymore so we can destroy it
    vmaDestroyBuffer(allocator, stagingBuffer, staging_alloc);
    vmaDestroyBuffer(allocator, indexStagingBuffer, indexstagingAlloc);

    //identity matrices
    componentRender.rotation_matrix[i] = glm::mat4(1);
    componentRender.scaling_matrix[i] = glm::mat4(1);
    

    componentRender.initialized[i] = true;

    //sort it into the last pipeline_id element
    for(int e=0; e < componentRender.entityIDs.size() && e != i; e++){
        if(componentRender.pipeline_id[i] < componentRender.pipeline_id[e]){
            // Command: Fill RenderObjectComponent; Format: moveIntoIndex(componentRender.{name}, e-1,i);
            moveIntoIndex(componentRender.degrees_acc, e-1,i);
            moveIntoIndex(componentRender.index_alloc, e-1,i);
            moveIntoIndex(componentRender.index_data, e-1,i);
            moveIntoIndex(componentRender.initialized, e-1,i);
            moveIntoIndex(componentRender.world_position, e-1,i);
            moveIntoIndex(componentRender.scaling_matrix, e-1,i);
            moveIntoIndex(componentRender.rotation_matrix, e-1,i);
            moveIntoIndex(componentRender.vertex_data, e-1,i);
            moveIntoIndex(componentRender.pipeline_id, e-1,i);
            moveIntoIndex(componentRender.texture, e-1,i);
            moveIntoIndex(componentRender.ubo_data, e-1,i);
            moveIntoIndex(componentRender.vertex_buffer, e-1,i);
            moveIntoIndex(componentRender.vertex_alloc, e-1,i);
            moveIntoIndex(componentRender.index_buffer, e-1,i);
            //End Command
        }
    }
}
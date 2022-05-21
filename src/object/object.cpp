#include <iostream>
#include "object.h"

WorldObject::WorldObject(){

    
    world_position.store(glm::vec3(0,0,0));
    scaling_matrix.store(glm::mat4(1.0f));
    rotation_matrix.store(glm::mat4(1.0f));
    initialized = false;

}


bool Vertex::operator==(Vertex& a){
    return (a.pos == pos) && (a.uv_coordinates == uv_coordinates);
}

 bool WorldObject::LoadObject(std::vector<float> vertices,std::vector<float> colors){
    //the vertices data include the UV coordinate data that must be in a different array alltogether
    //I convert the vector into a array because it's faster to acess the data in this way
    int size = vertices.size();
    if(!((size % 5)== 0)){
        printf("%d is not divisible by 5. Each vertex must take 5 arguments\n", size);
        return false;
    }
    int size_colors = vertices.size();

    if(!((size_colors % 3)== 0)){
        printf("%d is not divisible by 3. Each color must take 3 arguments\n", size);
        return false;
    }

    for(int i = 0; i < (size/5); i++){ //this assumes that the number of vertices is equal to the number of colors
        Vertex v;

        v.pos.x = vertices.at(i*5);
        v.pos.y = vertices.at((i*5)+1);
        v.pos.z = vertices.at((i*5)+2);
        v.uv_coordinates.x = vertices.at((i*5)+3);
        v.uv_coordinates.y = vertices.at((i*5)+4);


        v.color.x = colors.at(i*3);
        v.color.y = colors.at((i*3) + 1);
        v.color.z = colors.at((i*3) + 2);

        bool found = false;
        for (int i = 0; i < verticesdata.size(); i++){
            if(verticesdata[i] == v){
                found = true;
                indexdata.push_back(static_cast<uint16_t>(i));
                break;
            }
        }
        if (!found){
            verticesdata.emplace_back(v);
            indexdata.push_back(static_cast<uint16_t>(verticesdata.size() - 1));
            
        }
    }
    return true;
 }


void WorldObject::SendToGPU(VmaAllocator allocator, VulkanRenderer * renderer){
    //this will send to the gpu the list with vertex structs

    //first we will send the vertex data to a buffer on the cpu...
    VkBufferCreateInfo bufferinfo = {};
    bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferinfo.size = verticesdata.size() * sizeof(Vertex); //data in bytes
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
    memcpy(data, verticesdata.data(), verticesdata.size() * sizeof(Vertex));
    vmaUnmapMemory(allocator, staging_alloc);

    
    

    //now we create the buffer on the gpu
    VkBufferCreateInfo vertex_buffer_info = {};
    vertex_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertex_buffer_info.size = verticesdata.size() * sizeof(Vertex); //data in bytes
    vertex_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;


    vmamallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;


    if(vmaCreateBuffer(allocator, &vertex_buffer_info, &vmamallocInfo, &vertexbuffer, &alloc, nullptr) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create vertex buffer on the CPU...");
    }

    renderer->immediateSubmitTransfer(stagingBuffer, vertexbuffer,  verticesdata.size() * sizeof(Vertex));

    
    //index buffer



    VkBufferCreateInfo indexBufferStagingInfo = {};
    indexBufferStagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexBufferStagingInfo.size = indexdata.size() * sizeof(uint16_t); //data in bytes
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
    memcpy(ddata, indexdata.data(), indexdata.size() * sizeof(uint16_t));
    vmaUnmapMemory(allocator, indexstagingAlloc);

    //now we create the buffer on the gpu
    VkBufferCreateInfo indexBufferInfo = {};
    indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexBufferInfo.size = verticesdata.size() * sizeof(Vertex); //data in bytes
    indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;


    vmamallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;


    if(vmaCreateBuffer(allocator, &indexBufferInfo, &vmamallocInfo, &indexbuffer, &indexalloc, nullptr) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create vertex buffer on the CPU...");
    }

    renderer->immediateSubmitTransfer(indexStagingBuffer, indexbuffer,  indexdata.size() * sizeof(uint16_t));

    //we dont need the staging buffers anymore so we can destroy it
    vmaDestroyBuffer(allocator, stagingBuffer, staging_alloc);
    vmaDestroyBuffer(allocator, indexStagingBuffer, indexstagingAlloc);

    initialized = true;
}


glm::mat4 WorldObject::GetModelMatrix(){
    //Model matrix consists in 3 matrix multiplications, translation, rotation and scale IN THIS ORDER. Matrix Multiplication is not like 
    //normal multiplication and must be done in a specific order.

    //first, we define the translation matrix
    glm::mat4 translationmatrix = glm::translate(glm::mat4(1.0f), world_position.load());
    //now we get the model matrix, which represents the change from local vertices coordinates to world coordinates
    glm::mat4 modelmatrix = translationmatrix * rotation_matrix.load() * scaling_matrix.load();
    return modelmatrix;


}

void WorldObject::changeRotation(glm::vec3 rotation_degrees){
    glm::mat4 rmat = glm::mat4(1.0f);
    rmat = glm::rotate(rmat, rotation_degrees.x, glm::vec3(1,0,0));
    rmat = glm::rotate(rmat, rotation_degrees.y, glm::vec3(0,1,0));
    rmat = glm::rotate(rmat, rotation_degrees.z, glm::vec3(0,0,1));
    rotation_matrix.store(rmat);
}

void WorldObject::cleanup(VmaAllocator allocator){
    vmaDestroyBuffer(allocator, vertexbuffer, alloc);
    vmaDestroyBuffer(allocator, indexbuffer, indexalloc);

}


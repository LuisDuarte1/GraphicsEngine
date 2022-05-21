#ifndef OBJECT_H
#define OBJECT_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <atomic>
#include <functional>

#include <vulkan/vulkan.h>


class VulkanRenderer;
class VulkanTexture;
#include "../renderer/vulkan/vulkanrenderer.h"

#include "../../libs/VulkanMemoryAllocator/include/vk_mem_alloc.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#define GLM_FORCE_LEFT_HANDED 
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Vertex{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv_coordinates;

    bool operator==(Vertex& a);

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescription(){
        std::vector<VkVertexInputAttributeDescription> s;
        s.resize(3);
        s[0].binding = 0;
        s[0].location = 0;
        s[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        s[0].offset = offsetof(Vertex, pos);

        s[1].binding = 0;
        s[1].location = 1;
        s[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        s[1].offset = offsetof(Vertex, color);

        s[2].binding = 0;
        s[2].location = 2;
        s[2].format = VK_FORMAT_R32G32_SFLOAT;
        s[2].offset = offsetof(Vertex, uv_coordinates);


        return s;


    }



};

class WorldObject{
    public:
        WorldObject();

        std::vector<uint16_t> indexdata;
        VkBuffer indexbuffer;
        VmaAllocation indexalloc;
        
        std::vector<Vertex> verticesdata;
        VkBuffer vertexbuffer;

        double calc = 0;

        inline void ChangeWorldPosition(glm::vec3 newpos){world_position.store(newpos);};

        inline void setAlbedoTexture(VulkanTexture * newAlbedoTexture){albedoTexture = newAlbedoTexture;};
        inline VulkanTexture * getAlbedoTexture(){return albedoTexture;}; 

        void changeRotation(glm::vec3 rotation);
        


        void SendToGPU(VmaAllocator allocator, VulkanRenderer * renderer); //this should only be called when vertices are fully loaded
        void cleanup(VmaAllocator allocator);
        VmaAllocation alloc;


        unsigned width, height;

        glm::mat4 GetModelMatrix();

    

        bool LoadObject(std::vector<float> vertices ,std::vector<float> colors);
        bool initialized = false;

        int pipeline_id;

    private: 

        
        std::atomic<glm::vec3> world_position;
        //matrices specific to the object
        std::atomic<glm::mat4> scaling_matrix;
        //TODO: implement rotation for a specific object
        std::atomic<glm::mat4> rotation_matrix;

        VulkanTexture * albedoTexture = nullptr;

};

#endif

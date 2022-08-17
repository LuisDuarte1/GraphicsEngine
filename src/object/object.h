/*#ifndef OBJECT_H
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
        


        void InitializeToGPU(VmaAllocator allocator, VulkanRenderer * renderer); //this should only be called when vertices are fully loaded
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
*/
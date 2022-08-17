#ifndef OBJREADER_H
#define OBJREADER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#include <vulkan/vulkan.h>


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

std::vector<float> ReadObjFile(std::string path_to_obj);


std::vector<Vertex> LoadObject(std::vector<float> vertices,std::vector<float> colors, std::vector<uint16_t>& indexdata);

#endif
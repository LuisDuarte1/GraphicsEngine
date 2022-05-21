#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H


#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

class VulkanShader{

    public:
        VulkanShader(); 
        void readShaderFile(std::string filename);
        void createShaderModule(VkDevice device); //this should only be called when the shader file is read

        std::vector<char> buffer;
        VkShaderModule shader;


};

#endif
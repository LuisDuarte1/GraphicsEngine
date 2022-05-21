#include "vulkan-shader.h"

VulkanShader::VulkanShader(){

}


void VulkanShader::readShaderFile(std::string filename){
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t) file.tellg();
    VulkanShader::buffer.resize(fileSize);
    file.seekg(0);
    file.read(buffer.data(),fileSize); //TODO hash this to be used by a vulkan shader manager
    file.close();
}

void VulkanShader::createShaderModule(VkDevice device){
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shader) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
}


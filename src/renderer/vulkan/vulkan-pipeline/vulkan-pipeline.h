#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vulkan/vulkan.h>

#include "../../../libs/VulkanMemoryAllocator/include/vk_mem_alloc.h"


#include "../../../object/object.h"
#include "../vulkan-shader/vulkan-shader.h"

#include "../../../utils/objreader.h"


//there could be verious types that a uniform buffer can have, to make the system as dynamic as 
//possible, we need to specify the type to make dereferencing possible (because the pointer is a void pointer)

enum UniformBufferType{
    SIMPLE_TYPE
    
};

struct SimpleUniformBuffer{
    glm::mat4 proj_matrix;
};

//details on the physical device that are needed on the gpu
struct PipelineDetails{
    VkDeviceSize minBytesAlignmentDevice;
    uint32_t maxDescriptorSetUniformBuffersDynamic;
};


class VulkanPipeline{
    public:
        VulkanPipeline();
        void loadUniformBuffer(void * uniformBuffer, UniformBufferType bufferType, bool is_dynamic);
        void createUniformBuffersGPU(VkDevice device, VmaAllocator allocator, int max_size, PipelineDetails details);

        void updateUniformBuffer(VkDevice device, VmaAllocator allocator, void * newUniformBuffer, int n_frame, int id, int n_data);

        void loadShaders(std::string path_vert_shader, std::string path_frag_shader, VkDevice device);

        inline bool areShadersLoaded(){return shaders_loaded;};
        inline bool areUniformBuffersLoaded(){return bufferInitialized;};

        inline VkPipeline getPipeline(){return pipeline;};
        inline VkPipelineLayout getPipelineLayout(){return pipelineLayout;};
        inline std::vector<std::vector<VkDescriptorSet>> getDescriptorSet(){return descriptorSets;};
        inline std::vector<UniformBufferType> getBufferTypes(){return vertexUniformTypes;};
        inline std::vector<bool> getBufferDynamic(){return vertexUniformDynamic;};

        void createPipelineLayout(int w, int h, VkExtent2D swapChainExtent, VkDevice device, std::vector<VkDescriptorSetLayout> extraDSetLayouts); //this assumes default settings for pipeline
        void createPipeline(VkDevice device, VkRenderPass renderpass);
        void cleanup(VkDevice device, VmaAllocator allocator);

        int getSizeAligned(UniformBufferType type);
        int getSizeOfUniform(UniformBufferType type);

        int pipeline_id;


    private: 

        VkVertexInputBindingDescription bindingDescription;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        bool shaders_loaded = false;

        PipelineDetails pipelineDetails;

        VkPipeline pipeline; 
        VkPipelineLayout pipelineLayout;
        VulkanShader fragmentshader;
        VulkanShader vertexshader;
        std::vector<VkPipelineShaderStageCreateInfo> shaderstages;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkViewport viewport{};
        VkRect2D scissor{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};

        VkDescriptorPool descriptorPool;

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;


        std::vector< void *> vertexUniforms;
        std::vector<UniformBufferType> vertexUniformTypes;
        std::vector<bool> vertexUniformDynamic;


        //double vectors because we want to seperate if the different ubo buffers
        std::vector<std::vector<int>> vertexSizeBuffers;
        std::vector<std::vector<VkBuffer>> vertexUBOBuffers; 
        std::vector<std::vector<VmaAllocation>> vertexUBOAllocations;
        
        bool bufferInitialized = false;

        std::vector<std::vector<VkDescriptorSet>> descriptorSets;
        std::vector<std::vector<int>> descriptorSetSizes;
        
        void updateDescriptorSets(VkDevice device, int id, int n_frame ,int new_size);
        
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineDepthStencilStateCreateInfo getDepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);
        


        
};

#endif
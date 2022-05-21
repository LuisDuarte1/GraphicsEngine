#include "vulkan-pipeline.h"

VulkanPipeline::VulkanPipeline(){
    
}


void VulkanPipeline::loadShaders(std::string path_vert_shader, std::string path_frag_shader, VkDevice device){
    vertexshader.readShaderFile(path_vert_shader);
    fragmentshader.readShaderFile(path_frag_shader);
    vertexshader.createShaderModule(device);
    fragmentshader.createShaderModule(device);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexshader.shader;
    vertShaderStageInfo.pName = "main";
    shaderstages.push_back(vertShaderStageInfo);

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragmentshader.shader;
    fragShaderStageInfo.pName = "main";
    shaderstages.push_back(fragShaderStageInfo);

    VulkanPipeline::shaders_loaded = true;
}

void VulkanPipeline::createPipelineLayout(int w, int h, VkExtent2D swapChainExtent, VkDevice device, std::vector<VkDescriptorSetLayout> extraDSetLayouts){
    bindingDescription = Vertex::getBindingDescription();
    attributeDescriptions = Vertex::getAttributeDescription();


    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

    
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; //give triangles as input wihtout reuse, this is not very effficent but it can be changed in the future
    inputAssembly.primitiveRestartEnable = VK_FALSE;

     //fullscren view port
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) w;
    viewport.height = (float) h;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

     //we assume no multisampling for now
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional



    
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional


    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    //copy extra dsetlayouts into descriptorSetLayouts
    descriptorSetLayouts.insert(descriptorSetLayouts.end(), extraDSetLayouts.begin(), extraDSetLayouts.end());
    
    //finally create the pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size(); // Optional
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }


}

void VulkanPipeline::createPipeline(VkDevice device, VkRenderPass renderpass){
    //get depthStencil before
    depthStencil = getDepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderstages.size());
    pipelineInfo.pStages = shaderstages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional

    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderpass;
    pipelineInfo.subpass = 0;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragmentshader.shader, nullptr);
    vkDestroyShaderModule(device, vertexshader.shader, nullptr);

}



void VulkanPipeline::cleanup(VkDevice device,VmaAllocator allocator) {
    for(int i = 0; i < vertexUBOAllocations.size(); i++){
        for(int e = 0; e < vertexUBOAllocations[i].size(); e++){
            vmaDestroyBuffer(allocator, vertexUBOBuffers[i][e], vertexUBOAllocations[i][e]);
        }
    }
    for(VkDescriptorSetLayout descriptorSetLayout : descriptorSetLayouts){
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}


void VulkanPipeline::loadUniformBuffer(void * uniformBuffer, UniformBufferType bufferType, bool is_dynamic){
    //------
    vertexUniforms.push_back(uniformBuffer);
    vertexUniformTypes.push_back(bufferType);
    vertexUniformDynamic.push_back(is_dynamic);
    bufferInitialized = true;
}

void VulkanPipeline::createUniformBuffersGPU(VkDevice device, VmaAllocator allocator, int max_size, PipelineDetails details){


    if(VulkanPipeline::shaders_loaded == false && VulkanPipeline::bufferInitialized == false){
        throw std::runtime_error("Tried to create uniform buffers on the gpu before loading shader or tried to load uniform buffer type");
    }

    pipelineDetails = details;

    //create descriptor pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(max_size) * vertexUniforms.size();


    if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create Descriptor Pool...");
    }


    //first off, we need to create the descriptor set layout to give it to pipeline creation
    
    //resize vectors that store the buffers and allocations
    vertexUBOAllocations.resize(vertexUniformTypes.size());
    vertexUBOBuffers.resize(vertexUniformTypes.size());
    vertexSizeBuffers.resize(vertexUniformTypes.size());

    for(int e = 0; e < vertexUniformDynamic.size(); e++){
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        if(vertexUniformDynamic[e] == true){
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        }
        else{
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        VkDescriptorSetLayout descriptorSetLayout;
        if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS){
            throw std::runtime_error("couldn't create descriptor set layout");
        }

        descriptorSetLayouts.push_back(descriptorSetLayout);


    

     VkBufferCreateInfo bufferinfo = {};
        bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    
        //when adding a new type of uniform buffer, we should add it here to allow for creating a buffer with the correct size, always
        switch(vertexUniformTypes[e]){
            case SIMPLE_TYPE:
                bufferinfo.size = sizeof(SimpleUniformBuffer);//data in bytes
                break;
        }
        bufferinfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;


        VmaAllocationCreateInfo vmamallocInfo = {};
        vmamallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        std::vector<VkBuffer> uboBufferList(max_size);
        std::vector<VmaAllocation> uboAllocationList(max_size);
        std::vector<int> uboSizeBufferList(max_size, bufferinfo.size);


        for(int i = 0; i < max_size; i++){
            if (vmaCreateBuffer(allocator, &bufferinfo, &vmamallocInfo, &uboBufferList[i], &uboAllocationList[i], nullptr) != VK_SUCCESS){
                throw std::runtime_error("Couldn't create uniform buffer...");
            }
            
            
        }
        vertexUBOBuffers[e] = uboBufferList;
        vertexUBOAllocations[e] = uboAllocationList;
        vertexSizeBuffers[e] = uboSizeBufferList;

        std::vector<VkDescriptorSetLayout> layouts(max_size, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(max_size);
        allocInfo.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> dsets(max_size);
        int size = vertexUniformDynamic[e] == true ? getSizeAligned(vertexUniformTypes[e]) : getSizeOfUniform(vertexUniformTypes[e]);
        std::vector<int> dsetSizes(max_size, size + 1);
        
        VkResult dresult = vkAllocateDescriptorSets(device, &allocInfo, dsets.data());
        if(dresult != VK_SUCCESS){
            throw std::runtime_error("Failed to allocate desciptor sets");
        }
        descriptorSets.push_back(dsets);
        descriptorSetSizes.push_back(dsetSizes);
        

        for(int i = 0; i < max_size; i++){
            updateUniformBuffer(device, allocator, vertexUniforms[e], i, e, 1); //only put the first element
        }
    }    
}




int VulkanPipeline::getSizeAligned(UniformBufferType type){
    int type_size = getSizeOfUniform(type);
    if(type_size == -1){
        throw std::runtime_error("type wasn't in getSizeOfUniform Function");
    }
    int aligned_size = -1;
    int r = pipelineDetails.minBytesAlignmentDevice - (type_size % pipelineDetails.minBytesAlignmentDevice);
    return type_size + r;
}

int VulkanPipeline::getSizeOfUniform(UniformBufferType type){
    switch(type){
        case SIMPLE_TYPE:
            return sizeof(SimpleUniformBuffer);
    }
    return -1;
}

void VulkanPipeline::updateUniformBuffer(VkDevice device, VmaAllocator allocator ,void * newUniformBuffer, int n_frame, int id, int n_data){
    //this assumes the current type of the buffer
    if(!(0 <= n_frame && n_frame < vertexUBOAllocations[id].size())){
        throw std::runtime_error("The given frame number is bigger than the MAX_FRAMES_FLIGHT");
    }
    //TODO: check if id is dynamic and allocate memory accordingly
    if(!vertexUniformDynamic[id]){
        void * data;
        vmaMapMemory(allocator, vertexUBOAllocations[id][n_frame], &data);

        memcpy(data, newUniformBuffer, getSizeOfUniform(vertexUniformTypes[id]));
        vmaUnmapMemory(allocator, vertexUBOAllocations[id][n_frame]);
    } else {
        int size = getSizeOfUniform(vertexUniformTypes[id]);
        int size_aligned = getSizeAligned(vertexUniformTypes[id]);
        std::vector<char> data_to_copy;
        //alignment
        for(int i = 0; i < n_data; i++){
            //first get the bytes of the actual data on the buffer
            for(int e = 0; e < size; e++){
                data_to_copy.emplace_back(((char * )newUniformBuffer)[i*size + e]);
            }
            //then fill the rest with zeros
            while(data_to_copy.size() % size_aligned != 0){
                data_to_copy.push_back(0x00);
            }
        }

        
        //if the buffer has a different size than data_to_copy, we create destroy the buffer and create a new one
        //we can create it 
        if(vertexSizeBuffers[id][n_frame] < data_to_copy.size()){
            vmaDestroyBuffer(allocator, vertexUBOBuffers[id][n_frame], vertexUBOAllocations[id][n_frame]);
            VkBufferCreateInfo bufferinfo = {};
            bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferinfo.size = data_to_copy.size() + size_aligned * 10;  //create buffer for size and 10 more objects, this could be optimized with an equation to alloc according to expeted usage
            bufferinfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;


            VmaAllocationCreateInfo vmamallocInfo = {};
            vmamallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            if(vmaCreateBuffer(allocator,&bufferinfo, &vmamallocInfo,&vertexUBOBuffers[id][n_frame], &vertexUBOAllocations[id][n_frame], nullptr) != VK_SUCCESS){
                throw std::runtime_error("Couldn't recreate new dynamic buffer");
            }

        }

        void  * data;
        vmaMapMemory(allocator, vertexUBOAllocations[id][n_frame], &data);
        memcpy(data, data_to_copy.data(), data_to_copy.size());
        vmaUnmapMemory(allocator, vertexUBOAllocations[id][n_frame]);

        updateDescriptorSets(device, id, n_frame, data_to_copy.size());

    }
}


void VulkanPipeline::updateDescriptorSets(VkDevice device, int id, int n_frame ,int new_size){
            
        if(new_size == descriptorSetSizes[id][n_frame]){
            return; // no need to change
        }

            VkDescriptorBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = vertexUBOBuffers[id][n_frame];
            descriptorBufferInfo.offset = 0;
              
            
            //FIXME (luisd): apparently VK_WHOLE_SIZE doesnt work with offsetting very well, so we can instead
            // change the descriptor  to the new range if a object is added/deleted
            // only change the sets when it really has to, it's unnecessary to this every frame

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[id][n_frame];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            if(vertexUniformDynamic[id]){
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                descriptorBufferInfo.range = new_size;
            }
            else{
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorBufferInfo.range = new_size;
            }
            
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &descriptorBufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        
    
}


VkPipelineDepthStencilStateCreateInfo VulkanPipeline::getDepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp){

    VkPipelineDepthStencilStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;

    info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
    info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
    info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f; // Optional
    info.maxDepthBounds = 1.0f; // Optional
    info.stencilTestEnable = VK_FALSE;

    return info;
}
#include "vulkan-texture.h"
#include "../vulkanrenderer.h"

VulkanTexture::VulkanTexture(uint8_t * texture, uint32_t w, uint32_t h, VulkanRenderer * nrenderer, VkDescriptorPool * dpool, VkDescriptorSetLayout ndsetLayout){
    dsetLayout = ndsetLayout;
    //if it's ever initialized it should never be duplicated therefore it will create itself
    renderer = nrenderer; 
    descriptorPool = dpool;
    VkDeviceSize imageSize = w * h * 4; //we multiply by 4 because we expect 4 layers RGBA
    VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB; //RGBA in SRGB color space

    //now we create the staging buffer
     VkBufferCreateInfo imageBufferInfo = {};
    imageBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    imageBufferInfo.size = imageSize; //data in bytes
    imageBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo vmamallocInfo{};
    vmamallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VkBuffer stagingBuffer;
    VmaAllocation stagingAlloc;
    vmaCreateBuffer(renderer->allocator, &imageBufferInfo, &vmamallocInfo, &stagingBuffer, &stagingAlloc, nullptr);

    //copy texture data to the buffer
    void * data;
    vmaMapMemory(renderer->allocator, stagingAlloc, &data);
    memcpy(data, texture, static_cast<size_t>(imageSize));
    vmaUnmapMemory(renderer->allocator, stagingAlloc);

    //now we create the image in the  gpu memory

    VkExtent3D textureExtent;
    textureExtent.width = w;
    textureExtent.height = h;
    textureExtent.depth = 1;

    VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    VkImageCreateInfo imageCreateInfo = renderer->getImageCreateInfo(image_format,flags,textureExtent);
    imageCreateInfo.mipLevels = 1;

    vmamallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    
    VkResult t = vmaCreateImage(renderer->allocator, &imageCreateInfo, &vmamallocInfo, &image, &imageAlloc, nullptr);
    if( t != VK_SUCCESS){
        throw std::runtime_error("Couldn't create texture image on the gpu...");
    } 

    VkImageSubresourceRange range;
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    //queue families are required to transfer ownership
    //FIXME (luisd): this can probably be optimized to reduce vulkan calls made while creating a texture
    QueueFamilyIndices queueIndicies = renderer->findQueueFamilies(renderer->GetCurrentPhysicalDevice());

    renderer->customImmediateSubmitTransfer([&](VkCommandBuffer commandBuffer){


        VkImageMemoryBarrier imageBarrierTransfer{};
        imageBarrierTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrierTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageBarrierTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrierTransfer.image = image;
        imageBarrierTransfer.subresourceRange = range;
        

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,0,0,nullptr,0,nullptr,1,&imageBarrierTransfer);

        //after we have prepared the image for writing we can finally copy the buffer to the image

        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageExtent = textureExtent;

        vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        //Transfer queue families, needed by some gpus that don't share the transfer queue with the graphics queue (AKA, most discrete desktop gpu's)
        VkImageMemoryBarrier transferOwnership{};
        transferOwnership.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        transferOwnership.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transferOwnership.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transferOwnership.image = image;
        transferOwnership.subresourceRange = range;
        transferOwnership.srcQueueFamilyIndex = queueIndicies.transferFamily.value();
        transferOwnership.dstQueueFamilyIndex = queueIndicies.presentationFamily.value();

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,0,0,nullptr,0,nullptr,1,&transferOwnership);
        
    }, false);

    renderer->customImmediateSubmitTransfer([&](VkCommandBuffer commandBuffer){

        //after having the correct pixel data in the image we need to make it readable again

        VkImageMemoryBarrier imageBarrierReadable{};
        imageBarrierReadable.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrierReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrierReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageBarrierReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageBarrierReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        imageBarrierReadable.image = image;
        imageBarrierReadable.subresourceRange = range;


        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,0,0,nullptr,0,nullptr,1,&imageBarrierReadable);
    },true);

    VkImageViewCreateInfo imageViewInfo = renderer->getImageViewCreateInfo(image_format, image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(renderer->GetCurrentDevice(), &imageViewInfo, nullptr, &imageView);

    createImageSampler();

    createDescriptorSet();

    writeToDescriptorSet();

    //in the end we can destroy the staging buffer because it isn't needed anymore
    vmaDestroyBuffer(renderer->allocator, stagingBuffer, stagingAlloc);


}

void VulkanTexture::createDescriptorSet(){
    VkDescriptorSetAllocateInfo dsetAllocInfo{};
    dsetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsetAllocInfo.descriptorPool = *descriptorPool;
    dsetAllocInfo.descriptorSetCount = 1;
    dsetAllocInfo.pSetLayouts = &dsetLayout;
    if(vkAllocateDescriptorSets(renderer->GetCurrentDevice(), &dsetAllocInfo, &imageSet) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create texture descriptor set...");
    }
}

void VulkanTexture::createImageSampler(){
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    //use repeat texture if the u,v or w goes out of bounds ever.
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //use anisotrpy filtering to make the image look better
    samplerInfo.anisotropyEnable = VK_TRUE;
    //FIXME: get this value from properties because it will be hardcoded now
    samplerInfo.maxAnisotropy = 4;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    //use normalized coordinates because its standard and it comes with the obj object that way --> [0,1]
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if(vkCreateSampler(renderer->GetCurrentDevice(), &samplerInfo, nullptr, &imageSampler) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create image sampler...");
    }

}

void VulkanTexture::writeToDescriptorSet(){
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = imageSampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = imageSet;
    descriptorWrite.dstBinding = 1;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(renderer->GetCurrentDevice(), 1, &descriptorWrite, 0, nullptr);

}

void VulkanTexture::cleanup(){
    vkDestroySampler(renderer->GetCurrentDevice(), imageSampler, nullptr);
    vkDestroyImageView(renderer->GetCurrentDevice(), imageView, nullptr);
    vmaDestroyImage(renderer->allocator, image, imageAlloc);
}
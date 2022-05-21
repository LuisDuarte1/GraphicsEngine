#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vulkan/vulkan.h>

#include "../../../libs/VulkanMemoryAllocator/include/vk_mem_alloc.h"


#include "../vulkan-shader/vulkan-shader.h"

class VulkanRenderer;

struct TextureHash {
    uint8_t sha[32];
    bool operator==(const TextureHash &other) const
    {
        for(int i = 0; i < 32; i++){
            if(other.sha[i] != sha[i]){
                return false;
            }
        }
        return true;
    }
};

struct TextureHashHasher{
     std::size_t operator()(const TextureHash& k) const{
        std::size_t tt = std::hash<int>{}(static_cast<int>(k.sha[0]));
        for(int i = 1; i < 32; i++){
            tt = tt ^ (std::hash<int>{}(static_cast<int>(k.sha[i])) << i);
        }
        return tt;
     }
};

//we add types here, it will correspond to the descriptorSetLayout given to the texture
enum TextureType{
    ALBEDO_TEXTURE = 0
};


class VulkanTexture
{
public:
    VulkanTexture(uint8_t * texture, uint32_t w, uint32_t h, VulkanRenderer * nrenderer, VkDescriptorPool * dpool, VkDescriptorSetLayout ndsetLayout);
    //new texture hash must have 32 bytes in it
    inline VkDescriptorSet * getDescriptorSet(){return &imageSet;};
    inline void SetHash(TextureHash newTextureHash){textureHash = newTextureHash;}; 
    inline TextureHash GetHash(){return textureHash;};
    
    void cleanup();
private:
    void createDescriptorSet();

    void createImageSampler();

    void writeToDescriptorSet();

    VkDescriptorPool * descriptorPool;
    VulkanRenderer * renderer;

    VkImage image;
    VkImageView imageView;
    VkSampler imageSampler;

    VmaAllocation imageAlloc;

    VkDescriptorSet imageSet;

    VkDescriptorSetLayout dsetLayout;

    TextureHash textureHash; //every texture has a hash to avoid duplication in memory
    
    
};




#endif

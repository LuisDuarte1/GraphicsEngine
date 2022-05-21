#ifndef VULKAN_TEXTURE_MANAGER_H
#define VULKAN_TEXTURE_MANAGER_H

#include <vulkan/vulkan.h>

#include "../../../libs/VulkanMemoryAllocator/include/vk_mem_alloc.h"

#include "../../../utils/pngloader.h"
#include "vulkan-texture.h"
#include "../vulkan-shader/vulkan-shader.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <unordered_map>

#define MAX_DESCRIPTOR_SETS_PER_POOL 10



class VulkanTextureManager{
    public:
        VulkanTextureManager(VulkanRenderer * nrenderer, std::vector<VkDescriptorSetLayout> nextraDsetlayouts);
        VulkanTexture * AddTexture(std::string pathToTexture, TextureType ttype);
        void cleanup();
        

    private:
        void createDescriptorPool();
        VulkanRenderer * renderer;
        std::vector<VkDescriptorPool> textureDescriptorPools;
        std::vector<int> texturesInPools;

        std::vector<VkDescriptorSetLayout> extraDsetlayouts;

        std::unordered_map<TextureHash, VulkanTexture*, TextureHashHasher> textures;
};

#endif
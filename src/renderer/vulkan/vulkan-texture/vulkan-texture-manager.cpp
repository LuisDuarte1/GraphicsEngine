#include "vulkan-texture-manager.h"
#include "../vulkanrenderer.h"

VulkanTextureManager::VulkanTextureManager(VulkanRenderer * nrenderer,  std::vector<VkDescriptorSetLayout> nextraDsetlayouts){
    renderer = nrenderer;
    extraDsetlayouts = nextraDsetlayouts;
    //create the first descriptorPool
    createDescriptorPool();
}

VulkanTexture * VulkanTextureManager::AddTexture(std::string pathToTexture, TextureType ttype){
    std::tuple<std::vector<unsigned char>, unsigned, unsigned> textureFile = ReadPngFile(pathToTexture);
    TextureHash thash;
    EVP_Digest(std::get<0>(textureFile).data(), std::get<0>(textureFile).size(), thash.sha, nullptr, EVP_sha256(), nullptr);
    auto texture_found = textures.find(thash);
    if(texture_found == textures.end()){
        //couldn't find texture in hash map therefore we create a new one
        //and we check if the last descriptor pool is full, if it is we create a new one
        if(texturesInPools.back() == MAX_DESCRIPTOR_SETS_PER_POOL){
            createDescriptorPool();
        }
        VkDescriptorSetLayout dsetlayout;
        switch(ttype){
            case ALBEDO_TEXTURE:
                dsetlayout = extraDsetlayouts[0]; //it's always the first one in the list
                break;
        }
        VulkanTexture * new_Texture = new VulkanTexture(std::get<0>(textureFile).data(), std::get<1>(textureFile), 
            std::get<2>(textureFile), renderer, &textureDescriptorPools.back(), dsetlayout);
        textures.insert(std::make_pair(thash, new_Texture));
        //add one to the last element of the list
        new_Texture->SetHash(thash);
        *(texturesInPools.rbegin()) += 1;
        return new_Texture; 
    }
    return texture_found->second;
    
}

void VulkanTextureManager::createDescriptorPool(){
    VkDescriptorPoolSize newPoolSize{};
    newPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    newPoolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo newPoolInfo{};
    newPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    //allow to reset and free descriptor sets, for example if a texture isn't needed
    newPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; 
    newPoolInfo.maxSets = MAX_DESCRIPTOR_SETS_PER_POOL;
    newPoolInfo.poolSizeCount = 1;
    newPoolInfo.pPoolSizes = &newPoolSize;

    textureDescriptorPools.resize(textureDescriptorPools.size() + 1);
    if(vkCreateDescriptorPool(renderer->GetCurrentDevice(), &newPoolInfo, nullptr, &textureDescriptorPools[textureDescriptorPools.size()-1]) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create new texture descriptor pool");
    }
    texturesInPools.push_back(0);
}

void VulkanTextureManager::cleanup(){

}
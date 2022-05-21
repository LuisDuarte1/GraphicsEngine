#ifndef VULKAN_RENDERERHPP
#define VULKAN_RENDERERHPP
#define MAX_POINT_lights 6

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../../libs/VulkanMemoryAllocator/include/vk_mem_alloc.h"
#include "window/baseWindow.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#define GLM_FORCE_LEFT_HANDED 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <atomic>
#include <cstdint>
#include <algorithm>
#include <mutex>
#include <string.h>
#include <queue>
#include <chrono>
#include <set>
#include <optional>
#include <stdexcept>
#include <functional>
#include <unordered_map>


#include "../../camera/camera.h"
#include "../../light/light.h"

class VulkanPipeline;
struct PipelineDetails;
#include "vulkan-pipeline/vulkan-pipeline.h"
class VulkanTextureManager;
#include "vulkan-texture/vulkan-texture-manager.h"
class WorldObject;
#include "../../object/object.h"

const int MAX_FRAMES_IN_FLIGHT = 2;


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};




struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;
    std::optional<uint32_t> transferFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentationFamily.has_value() && transferFamily.has_value();
    }
};


class VulkanRenderer{
    public:
        VulkanRenderer(Window * newWindow);
        void render();
        void InitVulkan();
        void cleanup();

        //bool AddObjectToRender(WorldObject *objp);
        inline void SelectCurrentCamera(Camera* newcamera){current_camera.store(newcamera);}
        inline VkDevice GetCurrentDevice(){return device;};
        inline Camera* GetCurrentCamera(){return current_camera.load();};
        std::atomic<double> delta_time;

        VmaAllocator allocator;

        void AddPiplineToList(VulkanPipeline * pipeline); //pipeline should already have the shaders compiled before adding
        void createCommandBuffers();
        void updateCommandBuffer(int frame_id);

        PipelineDetails * pipelineDetails;
        void getPipelineDeviceParameters();

        bool AddObjectToRender(WorldObject *objp);

        
        //this is a simple copy buffer to buffer transfer using the transferpool
        void immediateSubmitTransfer(VkBuffer src, VkBuffer dst, size_t size);

        //this is a immediateSubmitTransfer that takes a lambda function as an argument
        //therefore, we can customize the transfer. This also uses the transferPool and can only do transfer operations
        void customImmediateSubmitTransfer(std::function<void(VkCommandBuffer)> func1, bool graphicsortransfer);

        VkImageCreateInfo getImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
        VkImageViewCreateInfo getImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);


        inline VulkanTextureManager * getTextureManager(){return tmanager;};
        //bool AddLightToRender(Light *lightp);
        //void GetNearbyLights(std::vector<Light*> * light_array, glm::vec3 current_pos);

        void drawFrame();



    private:

        Window * window;

        std::vector<const char*> InstanceExtensions = {"VK_EXT_debug_utils"};

    

        std::mutex object_list_mutex;
        std::vector<std::vector<WorldObject*>> object_list;
        void updateUBOs(int frame_id);

        bool checkValidationLayerSupport();

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char*> logicaldeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        

        #ifdef NDEBUG
            const bool enableValidationLayers = false;
        #else
            const bool enableValidationLayers = true;
        #endif

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void getRequiredExtensions();
        void setupDebugMessenger();

        void pickPhysicalDevice();
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice d);
        void createLogicalDevice();
        
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSurfaceKHR surface;
        void createSurface(); 

        SwapChainSupportDetails querySwapChainSupport();
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) ;
        void createSwapChain();
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkImageView> swapChainImageViews;
        void createImageViews();

        void createRenderPass();
        VkRenderPass renderPass;

        void createFramebuffers();
        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<VulkanPipeline*> pipelines;

        VkCommandPool commandPool;
        void createCommandPool();

        void createSemaphores();
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        
        
        void createAllocator();

        void createDescriptorPool();
        //VkDescriptorPool descriptorPool;

        
        size_t frame_count = 0; //FIXME: this shouldn't really be done this way 

        size_t currentFrame = 0;


        std::vector<VkCommandBuffer> commandBuffers;
        
        VkFence transferFence;
        VkCommandPool transferPool;
        VkCommandBuffer transferCommandBuffer;
        VkQueue transferQueue;

        VkFence singleGraphicsFence;

        uint32_t imageCount; //this is the count to create the correct number of descriptor sets, framebuffers, everything
        

        VkImage depthImage;
        VkImageView depthImageView;
        VmaAllocation depthImageAlloc;
        VkFormat depthFormat;
        VkFormat findSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags flags);
        void createDepthResources();
        
        std::vector<VkDescriptorSetLayout> extraDSetsLayout;
        void createExtraDescriptorSets();
        //binding = 1
        VkDescriptorSetLayout createAlbedoTextureDescriptorSet();

        void createVulkanTextureManager();
        VulkanTextureManager * tmanager;

        glm::mat4 proj_matrix;


        std::mutex graphicsMutex;
        std::mutex transferMutex;

        
        std::mutex object_mutex;
        //std::vector<WorldObject*> object_list;
        glm::mat4 projectionMatrix;
        std::atomic<Camera*> current_camera;
        //an unordered_map with the ProgramID as key should allow us to save some VULKAN calls even if it is a few hundreds of nanoseconds slower
        //std::unordered_map<GLuint, std::vector<WorldObject*>> objectshader_map;

        //Lights have a seperate list because we should render and/or get the info first to give the light info to the corresponding shader

        std::mutex light_mutex;
        //std::vector<Light*> lights_list;
};

#endif 
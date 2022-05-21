
#define VMA_IMPLEMENTATION
#include "../../libs/VulkanMemoryAllocator/include/vk_mem_alloc.h"

#include "vulkanrenderer.h"


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) { //get the srg color space because it's the most supported and the most accurate
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}


VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) { //use triple buffering if available if not use normal vsync
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        //TODO: get width and height from window
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}


SwapChainSupportDetails VulkanRenderer::querySwapChainSupport() {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }


    return details;
}


void VulkanRenderer::getRequiredExtensions() {
    //TODO: get extensions from window
    std::vector <const char * > rWindow = window->getRequiredExtensions();
    for(const char * r : rWindow) InstanceExtensions.push_back(r);
}

VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


void VulkanRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void VulkanRenderer::setupDebugMessenger(){
    if (!VulkanRenderer::enableValidationLayers) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VulkanRenderer::VulkanRenderer(Window * newWindow){
    window = newWindow;
}

void VulkanRenderer::InitVulkan(){
    //create instance
    if (VulkanRenderer::enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Graphics Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(InstanceExtensions.size());
    createInfo.ppEnabledExtensionNames = InstanceExtensions.data();
    if(VulkanRenderer::enableValidationLayers){
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else {
        createInfo.pNext = nullptr;
        createInfo.enabledLayerCount = 0;
    }


    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS){
        throw std::runtime_error("Failed to create instance!");
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "available extensions:\n";

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    //setup debug messenger
    VulkanRenderer::setupDebugMessenger();
    //pick the device aka the gpu
    VulkanRenderer::pickPhysicalDevice();

    //create surface
    VulkanRenderer::createSurface();

    //Create logical device
    VulkanRenderer::createLogicalDevice();

    VulkanRenderer::createSwapChain();

    VulkanRenderer::createImageViews();

    VulkanRenderer::createAllocator();


    VulkanRenderer::createDepthResources();

    VulkanRenderer::createRenderPass();
    
    //get pipelinedetails for creating pipelines later, alignment and max dynamic buffers
    VulkanRenderer::getPipelineDeviceParameters();

    VulkanRenderer::createFramebuffers();

    VulkanRenderer::createCommandPool();

    VulkanRenderer::createSemaphores();

    VulkanRenderer::createDescriptorPool();

    VulkanRenderer::createExtraDescriptorSets();

    VulkanRenderer::createVulkanTextureManager();

    

}

bool isDeviceSuitable(VkPhysicalDevice device){
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    printf("Checking GPU: %s \n", deviceProperties.deviceName);
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; //only allow to run in discrete gpu
}


void VulkanRenderer::pickPhysicalDevice(){
    VulkanRenderer::physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for(const auto& device: devices){
        if(isDeviceSuitable(device)){
           VulkanRenderer::physicalDevice = device;
            break;
        }
    }
    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

}

QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            if (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT){ //do not use the same queue family for transfers and graphics
                indices.transferFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (indices.isComplete()) {
                break;
            }

            i++;
        }
        indices.presentationFamily = indices.graphicsFamily;
        return indices;
    }


void VulkanRenderer::createLogicalDevice(){
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queues;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentationFamily.value(), indices.transferFamily.value()};
    float queuePriority = 1.0f;
    for(uint32_t queuefam : uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queuefam;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queues.push_back(queueCreateInfo);
    }


    //this is temporary more complex features might be needed
    VkPhysicalDeviceFeatures deviceFeatures{}; 
    //enable anisotropy to make images look better
    deviceFeatures.samplerAnisotropy = VK_TRUE;


    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queues.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queues.size());;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(logicaldeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = logicaldeviceExtensions.data();

    if (VulkanRenderer::enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    if(indices.graphicsFamily.value() == indices.presentationFamily.value()){
        presentQueue = graphicsQueue;
    } else{
        vkGetDeviceQueue(device, indices.presentationFamily.value(), 0, &presentQueue);
    }

    vkGetDeviceQueue(device, indices.transferFamily.value(), 0, &transferQueue);
        
}

void VulkanRenderer::cleanup(){

    //vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    for (std::vector<WorldObject *> objlist : object_list){
        for(WorldObject * obj : objlist){
            obj->cleanup(allocator);
        }
        
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);

    }
    vkDestroyFence(device, transferFence, nullptr);

    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyCommandPool(device, transferPool, nullptr);
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    //destroy every pipeline
    for(VulkanPipeline * pipeline : pipelines){
        pipeline->cleanup(device, allocator);
    }
    //destroy texture manager
    tmanager->cleanup();
    vmaDestroyAllocator(allocator);

    vkDestroyRenderPass(device, renderPass, nullptr);
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroyImageView(device, depthImageView, nullptr);
    vmaDestroyImage(allocator, depthImage, depthImageAlloc);
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    if (VulkanRenderer::enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(VulkanRenderer::instance, VulkanRenderer::debugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(VulkanRenderer::instance, nullptr);
    //TODO: send signal to destroy window
}


void VulkanRenderer::render(){
    //we need to get the delta time so we need to get the time that it took to draw the frame
    while(true){ //TODO: while it should render
        auto start = std::chrono::high_resolution_clock::now();
        drawFrame();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        delta_time.store(diff.count());
        frame_count++;
    }
    vkDeviceWaitIdle(device);
}

bool VulkanRenderer::checkValidationLayerSupport(){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : VulkanRenderer::validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;

}

void VulkanRenderer::createSurface(){
    window->createSurface(instance);
    surface = window->getSurface();
}


void VulkanRenderer::createSwapChain(){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) { //make sure we dont exceed the maxImageCount of the driver of the gpu
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; //how many layers the image has, unless the engine uses 3D this should be 1
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //this atribute means direct rendering to them, this should be changed for post processing
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentationFamily.value(), indices.transferFamily.value()};

    //they must be concurrent anyways because of the transfer queue
    if (indices.graphicsFamily != indices.presentationFamily) { //share the queue if the presentation queue and graphics queue are the same
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 3;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        uint32_t queueFamilyIndicesEqual[] = {indices.graphicsFamily.value(), indices.presentationFamily.value()};
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2; // Optional
        createInfo.pQueueFamilyIndices = queueFamilyIndicesEqual; // Optional
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform; //use the default transformation for the gpu
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; //when something is in front of the screen, the pixels should be obscured
    createInfo.oldSwapchain = VK_NULL_HANDLE; //this should be null when the swapchain is constant, this is invalid when rezising for example

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}


void VulkanRenderer::createImageViews(){
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void VulkanRenderer::createRenderPass(){
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.flags = 0;
    depthAttachment.format = depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depth_dependency = {};
    depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depth_dependency.dstSubpass = 0;
    depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.srcAccessMask = 0;
    depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};
    VkSubpassDependency dependencies[2] = {dependency, depth_dependency};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 2;
    renderPassInfo.pDependencies = dependencies;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}


void VulkanRenderer::AddPiplineToList(VulkanPipeline * pipeline){
    if(pipeline->areShadersLoaded() == false && pipeline->areUniformBuffersLoaded() == false){
        std::runtime_error("Tried to give the rendering engine not loaded shaders or not loaded initial uniform buffers");
    }
    //initialize the  pipeline before adding it to the list, this could be parellized easily
    pipeline->createUniformBuffersGPU(device, allocator, imageCount, *pipelineDetails);
    pipeline->createPipelineLayout(swapChainExtent.width, swapChainExtent.height, swapChainExtent, device, extraDSetsLayout);
    pipeline->createPipeline(device, renderPass);
    pipeline->pipeline_id = object_list.size();
    object_list.push_back({});
    pipelines.push_back(pipeline);
}


void VulkanRenderer::createFramebuffers(){
    swapChainFramebuffers.resize(swapChainImageViews.size());
    //create framebuffer for each imageview and add it to the list
    //each framebuffer has a specific render pass
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        proj_matrix = glm::perspective(float(glm::radians(45.0f)), (float)swapChainExtent.width/(float)swapChainExtent.height, 0.1f, 100.0f);
        //proj_matrix[1][1] *= -1;


        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanRenderer::createCommandPool(){
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
    VkCommandPoolCreateInfo graphicsPoolInfo{};
    graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    graphicsPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; //required to reset the command buffer
    if (vkCreateCommandPool(device, &graphicsPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
    VkCommandPoolCreateInfo transferPoolInfo{};
    transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transferPoolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
    transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
    if (vkCreateCommandPool(device, &transferPoolInfo, nullptr, &transferPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}



void VulkanRenderer::createCommandBuffers() {
    commandBuffers.resize(swapChainFramebuffers.size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}


void VulkanRenderer::createSemaphores() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) 
        {

            throw std::runtime_error("failed to create semaphores for a frame!");
        }
    }
    if (vkCreateFence(device, &fenceInfo, nullptr, &transferFence) != VK_SUCCESS){
        throw std::runtime_error("failed to create transfer queue fence!");
    }

    if(vkCreateFence(device, &fenceInfo, nullptr, &singleGraphicsFence) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create immediate graphics fence!");
    }
}


void VulkanRenderer::drawFrame(){
    graphicsMutex.lock();
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFences[currentFrame]);
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    //update every ubo
    for(std::vector<WorldObject *> obj_list_pipeline : object_list){
        for(WorldObject * obj : obj_list_pipeline){
            glm::vec3 rot;
            rot.x = 0;
            obj->calc = obj->calc + (delta_time * 2);
            if(obj->calc > 360){
                obj->calc = (delta_time * 2);

            }
            rot.y = obj->calc;
            rot.z = 0;
            obj->changeRotation(rot);
        }
    }
    updateUBOs(imageIndex);
    //then update the command buffer, because some attributes might have changed
    updateCommandBuffer(imageIndex);


    imagesInFlight[imageIndex] = inFlightFences[currentFrame];
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(presentQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    graphicsMutex.unlock();


}



void VulkanRenderer::createAllocator(){
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = device;
    allocatorInfo.instance = instance;
    vmaCreateAllocator(&allocatorInfo, &allocator);
}

bool VulkanRenderer::AddObjectToRender(WorldObject *objp){
    //this assumes that the object already has the vertex data and the color data initialized
    object_list_mutex.lock();
    
    object_list[objp->pipeline_id].push_back(objp);
    object_list_mutex.unlock();
    return true;
}

void VulkanRenderer::getPipelineDeviceParameters(){
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    VkPhysicalDeviceLimits limits = properties.limits;
    pipelineDetails = new PipelineDetails{};
    pipelineDetails->maxDescriptorSetUniformBuffersDynamic = limits.maxDescriptorSetUniformBuffersDynamic;
    pipelineDetails->minBytesAlignmentDevice = limits.minUniformBufferOffsetAlignment;


    
}


void VulkanRenderer::immediateSubmitTransfer(VkBuffer src, VkBuffer dst, size_t size){
    transferMutex.lock();
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = transferPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; 
    copyRegion.dstOffset = 0; 
    copyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create transfer command buffer...");
    }

    //after creating the buffer we will submit it to the gpu
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    vkResetFences(device, 1, &transferFence);
    VkResult t = vkQueueSubmit(transferQueue, 1, &submitInfo, transferFence);
    if(t != VK_SUCCESS ){
        throw std::runtime_error("Couldn't submit command buffer to queue");
    }

    vkWaitForFences(device, 1, &transferFence, VK_TRUE, 9999999999); //wait one second
    vkResetFences(device, 1, &transferFence);
    
    
    vkResetCommandPool(device, transferPool, 0);
    transferMutex.unlock();
}


void VulkanRenderer::updateUBOs(int frame_id){
    for(int i = 0; i < pipelines.size(); i++){
        std::vector<UniformBufferType> pipeline_types= pipelines[i]->getBufferTypes();
        std::vector<bool> pipeline_dynamic = pipelines[i]->getBufferDynamic();
        for(int e = 0; e < pipeline_types.size(); e++){
            if(pipeline_dynamic[e]){
                //if it's dynamic we need to update the whole thing, this could be optimized further by only
                //updating the necessary things by using offsets, I think there's no such need because:
                //let's say we have the mvp as a UBO, then if the camera changes every frame, we need to update
                //it all anyways 

                switch(pipeline_types[e]){
                    case(SIMPLE_TYPE):
                        std::vector<SimpleUniformBuffer> simple_ubo_list;
                        //we iterate by all objects and update it
                        for(int u = 0; u < object_list[i].size(); u++){
                            SimpleUniformBuffer simple_ubo;
                            simple_ubo.proj_matrix = proj_matrix * current_camera.load()->GetCameraMatrix() * object_list[i][u]->GetModelMatrix();
                            simple_ubo_list.push_back(simple_ubo);
                        }
                        pipelines[i]->updateUniformBuffer(device, allocator, simple_ubo_list.data(), 
                            frame_id,e, simple_ubo_list.size());
                        break;
                }
            } else {
                //if it's not dynamic we only need to update it once per all objects, therefore it's a constant thing
                //across all objects
            }
            
        }
    }
}

void VulkanRenderer::updateCommandBuffer(int frame_id){
    //first reset command buffer to use because vulkan will not let it update otherwise
    VkCommandBufferResetFlags flags = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
    if(vkResetCommandBuffer(commandBuffers[frame_id], flags) != VK_SUCCESS){
        throw std::runtime_error("failed to reset current frame command buffer");
    }
           VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[frame_id], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            VkClearValue colorClear =  {{{0.0f, 0.0f, 0.0f, 1.0f}}};


            VkClearValue depthClear;
            depthClear.depthStencil.depth = 1.f;

            VkClearValue clears[] = {colorClear, depthClear};

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFramebuffers[frame_id];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;
            renderPassInfo.clearValueCount = 2;
            renderPassInfo.pClearValues = clears;


            vkCmdBeginRenderPass(commandBuffers[frame_id], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        for(VulkanPipeline * pipeline : pipelines){

            vkCmdBindPipeline(commandBuffers[frame_id], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());


            for(int o = 0; o < object_list[pipeline->pipeline_id].size(); o++){
                WorldObject * obj = object_list[pipeline->pipeline_id][o];
                if(obj->initialized == false){ //if it's not initiliazed skip it entirely, this can be done on other threads
                    continue;
                }
                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(commandBuffers[frame_id], 0, 1, &(obj->vertexbuffer), &offset);
                std::vector<std::vector<VkDescriptorSet>> descriptorSets = pipeline->getDescriptorSet();
                std::vector<bool> DynamicSet = pipeline->getBufferDynamic();
                std::vector<UniformBufferType> buffertypes = pipeline->getBufferTypes();
                for(int e = 0; e < descriptorSets.size(); e++){
                    std::vector<VkDescriptorSet> dsets = descriptorSets[e];
                    
                    uint32_t dynamic_offsets = DynamicSet[e]==true ? pipeline->getSizeAligned(buffertypes[e]) * o : 0;
                    //uint32_t dynamic_offsets =  0;
                    uint32_t is_dynamic = DynamicSet[e]==true ? 1 : 0;
                    if(is_dynamic == 1){

                    }
                    vkCmdBindDescriptorSets(commandBuffers[frame_id], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipelineLayout(),
                        0, 1, &dsets[frame_id],is_dynamic, &dynamic_offsets);
                    
                    //bind albedo texture if it exists
                    VulkanTexture * albedo = obj->getAlbedoTexture();
                    if(albedo != nullptr){
                        vkCmdBindDescriptorSets(commandBuffers[frame_id], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipelineLayout(),
                            1,1, albedo->getDescriptorSet(),0,nullptr);
                        }
                }
                vkCmdBindIndexBuffer(commandBuffers[frame_id], obj->indexbuffer, 0, VK_INDEX_TYPE_UINT16);

                vkCmdDrawIndexed(commandBuffers[frame_id], obj->indexdata.size(), 1, 0, 0, 0);
            }
            

            vkCmdEndRenderPass(commandBuffers[frame_id]);
        }

    if (vkEndCommandBuffer(commandBuffers[frame_id]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}


void VulkanRenderer::createDescriptorPool(){

}

VkFormat VulkanRenderer::findSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags flags){
    for(VkFormat format : formats){
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
    
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & flags) == flags) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & flags) == flags) {
            return format;
        }
    }
    throw std::runtime_error("Couldn't find a supported format");   
}

VkImageCreateInfo VulkanRenderer::getImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent){
    VkImageCreateInfo info = { };
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    return info;
}

VkImageViewCreateInfo VulkanRenderer::getImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags){
    //build a image-view for the depth image to use for rendering
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;

	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.image = image;
	info.format = format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = aspectFlags;

	return info;
}




void VulkanRenderer::createDepthResources(){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport();
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    depthFormat = findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );  
    VkExtent3D depthExtent;
    depthExtent.depth = 1;
    depthExtent.height = extent.height;
    depthExtent.width = extent.width;
    VkImageCreateInfo depthImageInfo = getImageCreateInfo(depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthExtent);
    //after we create the extent and the depth image info we can create the image using vma

    //we want it to be gpu only because there's no use in sharing the depth buffer with the cpu
    VmaAllocationCreateInfo depthAllocationInfo = {};
	depthAllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	depthAllocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vmaCreateImage(allocator, &depthImageInfo, &depthAllocationInfo , &depthImage, &depthImageAlloc, nullptr);

    VkImageViewCreateInfo depthImageViewInfo = getImageViewCreateInfo(depthFormat, depthImage, VK_IMAGE_ASPECT_DEPTH_BIT);

    if(vkCreateImageView(device, &depthImageViewInfo, nullptr, &depthImageView) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create depth image view");
    }



}


void VulkanRenderer::customImmediateSubmitTransfer(std::function<void(VkCommandBuffer)> func1,  bool graphicsortransfer){
    //func1 must always have one argument, a VkCommandBuffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if(graphicsortransfer){
        graphicsMutex.lock();
        allocInfo.commandPool = commandPool;
    }else{
        transferMutex.lock();
        allocInfo.commandPool = transferPool;
    }
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    func1(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("Couldn't create transfer command buffer...");
    }

    //after creating the buffer we will submit it to the gpu
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    if(!graphicsortransfer){
        vkResetFences(device, 1, &transferFence);
        if(vkQueueSubmit(transferQueue, 1, &submitInfo, transferFence) != VK_SUCCESS ){
            throw std::runtime_error("Couldn't submit command buffer to queue");
        }

        vkWaitForFences(device, 1, &transferFence, VK_TRUE, 9999999999); //wait one second
        vkResetFences(device, 1, &transferFence);
        
        
        vkResetCommandPool(device, transferPool, 0);
        transferMutex.unlock();
    } else{
        vkResetFences(device, 1, &singleGraphicsFence);
        if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, singleGraphicsFence) != VK_SUCCESS ){
            throw std::runtime_error("Couldn't submit command buffer to queue");
        }

        vkWaitForFences(device, 1, &singleGraphicsFence, VK_TRUE, 9999999999); //wait one second
        vkResetFences(device, 1, &singleGraphicsFence);
        
        
        vkResetCommandPool(device, transferPool, 0);   
        graphicsMutex.unlock(); 
    }
}


void VulkanRenderer::createExtraDescriptorSets(){
    extraDSetsLayout.emplace_back(createAlbedoTextureDescriptorSet());
}

VkDescriptorSetLayout VulkanRenderer::createAlbedoTextureDescriptorSet(){
    //Albedo textures always have a binding of 1
    VkDescriptorSetLayout dsetLayout;

    VkDescriptorSetLayoutBinding dsetBindings{};
    dsetBindings.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; 
    dsetBindings.descriptorCount = 1;
    dsetBindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dsetBindings.binding = 1;
    dsetBindings.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo dsetLayoutInfo{};
    dsetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsetLayoutInfo.bindingCount = 1;
    dsetLayoutInfo.pBindings = &dsetBindings;
    
    vkCreateDescriptorSetLayout(device, &dsetLayoutInfo, nullptr, &dsetLayout);
    return dsetLayout;
}


void VulkanRenderer::createVulkanTextureManager(){
    tmanager = new VulkanTextureManager(this, extraDSetsLayout);

}

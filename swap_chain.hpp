#pragma once
#include"createBufferHelper.hpp"
#include"uv.hpp"
#include"picture.hpp"
class Swap_Chain {
public:
    std::shared_ptr<Device>device;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    uint32_t currentFrame = 0;
    //present
    VkImage presentImage;
    VkDeviceMemory presentImageMemory;
    VkImageView presentImageView;
    //test depth
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    //light depth
    VkImage shadowImage;
    VkDeviceMemory shadowImageMemory;
    VkImageView shadowImageView;
    //camera depth
    VkImage cameraImage;
    VkDeviceMemory cameraImageMemory;
    VkImageView cameraImageView;
    //after deal
    VkImage afterImage;
    VkDeviceMemory afterImageMemory;
    VkImageView afterImageView;
    //shadow data
    VkImage shadowDataImage;
    VkDeviceMemory shadowDataImageMemory;
    VkImageView shadowDataImageView;
    //position buffer
    VkImage positionTexture;
    VkDeviceMemory positionTextureMemory;
    VkImageView positionTextureView;
    //normal buffer
    VkImage normalTexture;
    VkDeviceMemory normalTextureMemory;
    VkImageView normalTextureView;
    
    Swap_Chain(std::shared_ptr<Device>device) :device{ device } {

        createSwapChain();
        createImageViews();
        createRenderPass();
        createColorResources();
        createDepthResources();

        createShadowResources();
        createCameraResources();

        createAfterResources();
        createShadowDataResources();

        createPositionTexture();
        createNormalTexture();

       
        createFramebuffers();

    }
    ~Swap_Chain() {
        vkDestroyImageView(device.get()->device, normalTextureView, nullptr);
        vkDestroyImage(device.get()->device, normalTexture, nullptr);
        vkFreeMemory(device.get()->device, normalTextureMemory, nullptr);

        vkDestroyImageView(device.get()->device, positionTextureView, nullptr);
        vkDestroyImage(device.get()->device, positionTexture, nullptr);
        vkFreeMemory(device.get()->device, positionTextureMemory, nullptr);

        vkDestroyImageView(device.get()->device, shadowDataImageView, nullptr);
        vkDestroyImage(device.get()->device, shadowDataImage, nullptr);
        vkFreeMemory(device.get()->device, shadowDataImageMemory, nullptr);

        vkDestroyImageView(device.get()->device, afterImageView, nullptr);
        vkDestroyImage(device.get()->device, afterImage, nullptr);
        vkFreeMemory(device.get()->device, afterImageMemory, nullptr);

        vkDestroyImageView(device.get()->device, cameraImageView, nullptr);
        vkDestroyImage(device.get()->device, cameraImage, nullptr);
        vkFreeMemory(device.get()->device, cameraImageMemory, nullptr);

        vkDestroyImageView(device.get()->device, shadowImageView, nullptr);
        vkDestroyImage(device.get()->device, shadowImage, nullptr);
        vkFreeMemory(device.get()->device, shadowImageMemory, nullptr);

        cleanupSwapChain();
        vkDestroyRenderPass(device.get()->device, renderPass, nullptr);
    }
    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = device.get()->querySwapChainSupport(device->physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = device->surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = device->findQueueFamilies(device->physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(device.get()->device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device.get()->device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.get()->device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1,device.get());
        }
    }
    void createFramebuffers() {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<VkImageView, 8> attachments = {
                afterImageView,
                depthImageView,
                shadowImageView,
                cameraImageView,
                swapChainImageViews[i],
                shadowDataImageView,
                positionTextureView,
                normalTextureView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device.get()->device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
    void createRenderPass() {
        VkAttachmentDescription afterAttachment{};
        afterAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        afterAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        afterAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        afterAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        afterAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        afterAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        afterAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        afterAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription shadowAttachment{};
        shadowAttachment.format = findDepthFormat();
        shadowAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        shadowAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        shadowAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        shadowAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        shadowAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        shadowAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        shadowAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentDescription cameraAttachment{};
        cameraAttachment.format = findDepthFormat();
        cameraAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        cameraAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        cameraAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        cameraAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        cameraAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        cameraAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        cameraAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentDescription presentAttachment{};
        presentAttachment.format = swapChainImageFormat;
        presentAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        presentAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        presentAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        presentAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        presentAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        presentAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        presentAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription shadowDataAttachment{};
        shadowDataAttachment.format = VK_FORMAT_R32G32_SFLOAT;
        shadowDataAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        shadowDataAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        shadowDataAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        shadowDataAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        shadowDataAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        shadowDataAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        shadowDataAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentDescription positionAttachment{};
        positionAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        positionAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        positionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        positionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        positionAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        positionAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        positionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        positionAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentDescription normalAttachment{};
        normalAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        normalAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        normalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        normalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        normalAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        normalAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        normalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        //subpass0 create shadow map
        VkAttachmentReference shadowAttachmentRef{};
        shadowAttachmentRef.attachment = 2;
        shadowAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference shadowDataAttachmentRef{};
        shadowDataAttachmentRef.attachment = 5;
        shadowDataAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        std::array<VkAttachmentReference, 1> subpass0Out = { shadowDataAttachmentRef };

        VkSubpassDescription subpass0{};
        subpass0.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass0.colorAttachmentCount = subpass0Out.size();
        subpass0.pColorAttachments = subpass0Out.data();
        subpass0.pDepthStencilAttachment = &shadowAttachmentRef;

        //subpass1 create position&normal buffers
        VkAttachmentReference cameraAttachmentRef{};
        cameraAttachmentRef.attachment = 3;
        cameraAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        
        VkAttachmentReference positionAttachmentRef{};
        positionAttachmentRef.attachment = 6;
        positionAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference normalAttachmentRef{};
        normalAttachmentRef.attachment = 7;
        normalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        std::array<VkAttachmentReference, 2> subpass1Out = { positionAttachmentRef, normalAttachmentRef };
        
        VkSubpassDescription subpass1{};
        subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass1.colorAttachmentCount = subpass1Out.size();
        subpass1.pColorAttachments = subpass1Out.data();
        subpass1.pDepthStencilAttachment = &cameraAttachmentRef;

        //subpass2
        VkAttachmentReference depthRef{};
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference shadowInput{};
        shadowInput.attachment = 2;
        shadowInput.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference shadowDataInput{};
        shadowDataInput.attachment = 5;
        shadowDataInput.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference outColor{};
        outColor.attachment = 0;
        outColor.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        std::array<VkAttachmentReference, 2> subpass2In = { shadowInput, shadowDataInput };
        std::array<VkAttachmentReference, 1> subpass2Out = { outColor };

        VkSubpassDescription subpass2{};
        subpass2.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass2.inputAttachmentCount = subpass2In.size();
        subpass2.pInputAttachments = subpass2In.data();
        subpass2.colorAttachmentCount = subpass2Out.size();
        subpass2.pColorAttachments = subpass2Out.data();
        subpass2.pDepthStencilAttachment = &depthRef;

        //subpass3
        VkAttachmentReference afterInput{};
        afterInput.attachment = 0;
        afterInput.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference depthInput{};
        depthInput.attachment = 3;
        depthInput.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        VkAttachmentReference positionInput{};
        positionInput.attachment = 6;
        positionInput.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference normalInput{};
        normalInput.attachment = 7;
        normalInput.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        VkAttachmentReference resultOut{};
        resultOut.attachment = 4;
        resultOut.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        std::array<VkAttachmentReference, 4> subpass3In = { afterInput, depthInput, positionInput, normalInput};
        std::array<VkAttachmentReference, 1> subpass3Out = { resultOut };

        VkSubpassDescription subpass3{};
        subpass3.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass3.inputAttachmentCount = subpass3In.size();
        subpass3.pInputAttachments = subpass3In.data();
        subpass3.colorAttachmentCount = subpass3Out.size();
        subpass3.pColorAttachments = subpass3Out.data();

        

        VkSubpassDependency dependency0{};
        dependency0.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency0.dstSubpass = 0;
        dependency0.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency0.srcAccessMask = 0;
        dependency0.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency0.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency dependency1{};
        dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency1.dstSubpass = 1;
        dependency1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency1.srcAccessMask = 0;
        dependency1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency dependency2{};
        dependency2.srcSubpass = 0;
        dependency2.dstSubpass = 2;
        dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency2.srcAccessMask = 0;
        dependency2.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency2.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency dependency3{};
        dependency3.srcSubpass = 1;
        dependency3.dstSubpass = 3;
        dependency3.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency3.srcAccessMask = 0;
        dependency3.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency3.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        
        VkSubpassDependency dependency4{};
        dependency4.srcSubpass = 2;
        dependency4.dstSubpass = 3;
        dependency4.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency4.srcAccessMask = 0;
        dependency4.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency4.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        

        std::array<VkAttachmentDescription, 8> attachments = { afterAttachment, depthAttachment, shadowAttachment, cameraAttachment, presentAttachment, shadowDataAttachment, positionAttachment, normalAttachment };
        std::array<VkSubpassDescription, 4>subpasses = { subpass0, subpass1, subpass2, subpass3 };
        std::array<VkSubpassDependency, 5>dependencies = { dependency0 ,dependency1 ,dependency2 ,dependency3 ,dependency4 };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = subpasses.size();
        renderPassInfo.pSubpasses = subpasses.data();
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(device.get()->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void createColorResources() {
        VkFormat colorFormat = swapChainImageFormat;

        createImage(
            swapChainExtent.width, swapChainExtent.height, 
            1, 
            VK_SAMPLE_COUNT_1_BIT,
            colorFormat, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            presentImage, 
            presentImageMemory,
            device.get()
        );
        presentImageView = createImageView(presentImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1,device.get());
    }
    //
    
    //
    

    void createDepthResources() {
        VkFormat depthFormat = findDepthFormat();

        createImage(
            swapChainExtent.width, swapChainExtent.height, 
            1, 
            VK_SAMPLE_COUNT_1_BIT,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT| VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            depthImage, 
            depthImageMemory,
            device.get()
        );
        depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1,device.get());
    }

    void createShadowResources() {
        VkFormat depthFormat = findDepthFormat();

        createImage(
            swapChainExtent.width, swapChainExtent.height,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT| VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT| VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            shadowImage,
            shadowImageMemory,
            device.get()
        );
        shadowImageView = createImageView(shadowImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, device.get());
    }
    
    void createCameraResources() {
        VkFormat depthFormat = findDepthFormat();

        createImage(
            swapChainExtent.width, swapChainExtent.height,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            cameraImage,
            cameraImageMemory,
            device.get()
        );
        cameraImageView = createImageView(cameraImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, device.get());
    }
    void createAfterResources() {
        VkFormat afterFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

        createImage(
            swapChainExtent.width, swapChainExtent.height,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            afterFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            afterImage,
            afterImageMemory,
            device.get()
        );
        afterImageView = createImageView(afterImage, afterFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device.get());
    }

    void createPositionTexture() {
        VkFormat positionFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

        createImage(
            swapChainExtent.width, swapChainExtent.height,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            positionFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            positionTexture,
            positionTextureMemory,
            device.get()
        );
        positionTextureView = createImageView(positionTexture, positionFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device.get());

    }
    void createNormalTexture() {
        VkFormat normalFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

        createImage(
            swapChainExtent.width, swapChainExtent.height,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            normalFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            normalTexture,
            normalTextureMemory,
            device.get()
        );
        normalTextureView = createImageView(normalTexture, normalFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device.get());

    }


    void createShadowDataResources() {
        VkFormat dataFormat = VK_FORMAT_R32G32_SFLOAT;

        createImage(
            swapChainExtent.width, swapChainExtent.height,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            dataFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            shadowDataImage,
            shadowDataImageMemory,
            device.get()
        );
        shadowDataImageView = createImageView(shadowDataImage, dataFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device.get());
    }

   
    void cleanupSwapChain() {

        vkDestroyImageView(device.get()->device, depthImageView, nullptr);
        vkDestroyImage(device.get()->device, depthImage, nullptr);
        vkFreeMemory(device.get()->device, depthImageMemory, nullptr);

        vkDestroyImageView(device.get()->device, presentImageView, nullptr);
        vkDestroyImage(device.get()->device, presentImage, nullptr);
        vkFreeMemory(device.get()->device, presentImageMemory, nullptr);

        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device.get()->device, framebuffer, nullptr);
        }

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device.get()->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device.get()->device, swapChain, nullptr);
    }
    //helper
    

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }
        

        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(device.get()->window.get()->window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkFormat findDepthFormat() {
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }
   
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(device.get()->physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }
   
    void recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(device.get()->window.get()->window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(device.get()->window.get()->window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device->device);

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createColorResources();
        createDepthResources();
        createFramebuffers();


    }
};
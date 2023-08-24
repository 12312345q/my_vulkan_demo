#pragma once
#include"descriptors.hpp"
#include"mesh.hpp"
#include"picture.hpp"
class PipeLine {
public:

    std::shared_ptr<Device>device;

    std::vector<VkPipelineLayout> pipelineLayouts;
    std::vector<VkPipeline> graphicsPipelines;




    PipeLine(
        std::shared_ptr<Device>device,
        std::shared_ptr<Swap_Chain> swapchain,
        std::shared_ptr<Layout> layout
    ):device{ device } {
        createGraphicsPipeline(swapchain.get(), layout.get());
    }
    
    ~PipeLine() {
        for (auto i : graphicsPipelines) {
            vkDestroyPipeline(device.get()->device, i, nullptr);
        }
        for (auto i : pipelineLayouts) {
            vkDestroyPipelineLayout(device.get()->device, i, nullptr);
        }
        
    }


    void createGraphicsPipeline(Swap_Chain* swapchain,Layout* layout) {
        pipelineLayouts.resize(layout->descriptorSetLayouts.size());
        graphicsPipelines.resize(layout->descriptorSetLayouts.size());
        
        
        
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState shadowBlendAttachment{};
        shadowBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT ;
        shadowBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo shadowBlending{};
        shadowBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        shadowBlending.logicOpEnable = VK_FALSE;
        shadowBlending.logicOp = VK_LOGIC_OP_COPY;
        shadowBlending.attachmentCount = 1;
        shadowBlending.pAttachments = &shadowBlendAttachment;
        shadowBlending.blendConstants[0] = 0.0f;
        shadowBlending.blendConstants[1] = 0.0f;


        
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;


        VkPipelineColorBlendAttachmentState positionBlendAttachment{};
        positionBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        positionBlendAttachment.blendEnable = VK_FALSE;
        
 
        VkPipelineColorBlendAttachmentState normalBlendAttachment{};
        normalBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        normalBlendAttachment.blendEnable = VK_FALSE;

        std::array<VkPipelineColorBlendAttachmentState, 2>attachments = { positionBlendAttachment ,normalBlendAttachment };
        VkPipelineColorBlendStateCreateInfo buffersBlending{};
        buffersBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        buffersBlending.logicOpEnable = VK_FALSE;
        buffersBlending.logicOp = VK_LOGIC_OP_COPY;
        buffersBlending.attachmentCount = attachments.size();
        buffersBlending.pAttachments = attachments.data();
        buffersBlending.blendConstants[0] = 0.0f;
        buffersBlending.blendConstants[1] = 0.0f;
        buffersBlending.blendConstants[2] = 0.0f;
        buffersBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPushConstantRange pushconstantRange{};
        pushconstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushconstantRange.offset = 0;
        pushconstantRange.size = sizeof(pushConstant);

        for (auto i = 0; i < pipelineLayouts.size(); i++) {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &layout->descriptorSetLayouts[i];
            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &pushconstantRange;

            if (vkCreatePipelineLayout(device.get()->device, &pipelineLayoutInfo, nullptr, &pipelineLayouts[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
        }
        //pipeline0
        auto shadowVertShaderCode = readFile(SHADOW_VERTEX_SHADER_PATH);
        auto shadowFragShaderCode = readFile(SHADOW_FRAGMENT_SHADER_PATH);

        VkShaderModule shadowVertShaderModule = createShaderModule(shadowVertShaderCode);
        VkShaderModule shadowFragShaderModule = createShaderModule(shadowFragShaderCode);

        VkPipelineShaderStageCreateInfo shadowVertShaderStageInfo{};
        shadowVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shadowVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shadowVertShaderStageInfo.module = shadowVertShaderModule;
        shadowVertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shadowFragShaderStageInfo{};
        shadowFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shadowFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shadowFragShaderStageInfo.module = shadowFragShaderModule;
        shadowFragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shadowShaderStages[] = { shadowVertShaderStageInfo, shadowFragShaderStageInfo };
        VkGraphicsPipelineCreateInfo shadowPipelineInfo{};
        shadowPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        shadowPipelineInfo.stageCount = 2;
        shadowPipelineInfo.pStages = shadowShaderStages;
        shadowPipelineInfo.pVertexInputState = &vertexInputInfo;
        shadowPipelineInfo.pInputAssemblyState = &inputAssembly;
        shadowPipelineInfo.pViewportState = &viewportState;
        shadowPipelineInfo.pRasterizationState = &rasterizer;
        shadowPipelineInfo.pColorBlendState = &shadowBlending;
        shadowPipelineInfo.pMultisampleState = &multisampling;
        shadowPipelineInfo.pDepthStencilState = &depthStencil;
        shadowPipelineInfo.pDynamicState = &dynamicState;
        shadowPipelineInfo.layout = pipelineLayouts[0];
        shadowPipelineInfo.renderPass = swapchain->renderPass;
        shadowPipelineInfo.subpass = 0;
        shadowPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device.get()->device, VK_NULL_HANDLE, 1, &shadowPipelineInfo, nullptr, &graphicsPipelines[0]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device.get()->device, shadowFragShaderModule, nullptr);
        vkDestroyShaderModule(device.get()->device, shadowVertShaderModule, nullptr);
        
        //pipeline1

        auto depthVertShaderCode = readFile(CAMERA_VERTEX_SHADER_PATH);
        auto depthFragShaderCode = readFile(CAMERA_FRAGMENT_SHADER_PATH);

        VkShaderModule depthVertShaderModule = createShaderModule(depthVertShaderCode);
        VkShaderModule depthFragShaderModule = createShaderModule(depthFragShaderCode);

        VkPipelineShaderStageCreateInfo depthVertShaderStageInfo{};
        depthVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        depthVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        depthVertShaderStageInfo.module = depthVertShaderModule;
        depthVertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo depthFragShaderStageInfo{};
        depthFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        depthFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        depthFragShaderStageInfo.module = depthFragShaderModule;
        depthFragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo depthShaderStages[] = { depthVertShaderStageInfo, depthFragShaderStageInfo };
        VkGraphicsPipelineCreateInfo depthPipelineInfo{};
        depthPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        depthPipelineInfo.stageCount = 2;
        depthPipelineInfo.pStages = depthShaderStages;
        depthPipelineInfo.pVertexInputState = &vertexInputInfo;
        depthPipelineInfo.pInputAssemblyState = &inputAssembly;
        depthPipelineInfo.pViewportState = &viewportState;
        depthPipelineInfo.pRasterizationState = &rasterizer;
        depthPipelineInfo.pMultisampleState = &multisampling;
        depthPipelineInfo.pDepthStencilState = &depthStencil;
        depthPipelineInfo.pColorBlendState = &buffersBlending;
        depthPipelineInfo.pDynamicState = &dynamicState;
        depthPipelineInfo.layout = pipelineLayouts[1];
        depthPipelineInfo.renderPass = swapchain->renderPass;
        depthPipelineInfo.subpass = 1;
        depthPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device.get()->device, VK_NULL_HANDLE, 1, &depthPipelineInfo, nullptr, &graphicsPipelines[1]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device.get()->device, depthFragShaderModule, nullptr);
        vkDestroyShaderModule(device.get()->device, depthVertShaderModule, nullptr);


        //pipeline2.

        auto preVertShaderCode = readFile(LIGHT_VERTEX_SHADER_PATH);
        auto preFragShaderCode = readFile(LIGHT_FRAGMENT_SHADER_PATH);

        VkShaderModule preVertShaderModule = createShaderModule(preVertShaderCode);
        VkShaderModule preFragShaderModule = createShaderModule(preFragShaderCode);

        VkPipelineShaderStageCreateInfo preVertShaderStageInfo{};
        preVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        preVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        preVertShaderStageInfo.module = preVertShaderModule;
        preVertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo preFragShaderStageInfo{};
        preFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        preFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        preFragShaderStageInfo.module = preFragShaderModule;
        preFragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo preShaderStages[] = { preVertShaderStageInfo, preFragShaderStageInfo };
        VkGraphicsPipelineCreateInfo prePipelineInfo{};
        prePipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        prePipelineInfo.stageCount = 2;
        prePipelineInfo.pStages = preShaderStages;
        prePipelineInfo.pVertexInputState = &vertexInputInfo;
        prePipelineInfo.pInputAssemblyState = &inputAssembly;
        prePipelineInfo.pViewportState = &viewportState;
        prePipelineInfo.pRasterizationState = &rasterizer;
        prePipelineInfo.pMultisampleState = &multisampling;
        prePipelineInfo.pDepthStencilState = &depthStencil;
        prePipelineInfo.pColorBlendState = &colorBlending;
        prePipelineInfo.pDynamicState = &dynamicState;
        prePipelineInfo.layout = pipelineLayouts[2];
        prePipelineInfo.renderPass = swapchain->renderPass;
        prePipelineInfo.subpass = 2;
        prePipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device.get()->device, VK_NULL_HANDLE, 1, &prePipelineInfo, nullptr, &graphicsPipelines[2]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device.get()->device, preFragShaderModule, nullptr);
        vkDestroyShaderModule(device.get()->device, preVertShaderModule, nullptr);

        //pipeline3

        VkPipelineVertexInputStateCreateInfo anotherVertexInputInfo{};
        anotherVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto anotherBindingDescription = Picture::getBindingDescription();
        auto anotherAttributeDescriptions = Picture::getAttributeDescriptions();

        anotherVertexInputInfo.vertexBindingDescriptionCount = 1;
        anotherVertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(anotherAttributeDescriptions.size());
        anotherVertexInputInfo.pVertexBindingDescriptions = &anotherBindingDescription;
        anotherVertexInputInfo.pVertexAttributeDescriptions = anotherAttributeDescriptions.data();

        auto processVertShaderCode = readFile(PROCESSING_VERTEX_SHADER_PATH);
        auto processFragShaderCode = readFile(PROCESSING_FRAGMENT_SHADER_PATH);

        VkShaderModule processVertShaderModule = createShaderModule(processVertShaderCode);
        VkShaderModule processFragShaderModule = createShaderModule(processFragShaderCode);

        VkPipelineShaderStageCreateInfo processVertShaderStageInfo{};
        processVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        processVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        processVertShaderStageInfo.module = processVertShaderModule;
        processVertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo processFragShaderStageInfo{};
        processFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        processFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        processFragShaderStageInfo.module = processFragShaderModule;
        processFragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo processShaderStages[] = { processVertShaderStageInfo, processFragShaderStageInfo };
        VkGraphicsPipelineCreateInfo processPipelineInfo{};
        processPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        processPipelineInfo.stageCount = 2;
        processPipelineInfo.pStages = processShaderStages;
        processPipelineInfo.pVertexInputState = &anotherVertexInputInfo;
        processPipelineInfo.pInputAssemblyState = &inputAssembly;
        processPipelineInfo.pViewportState = &viewportState;
        processPipelineInfo.pRasterizationState = &rasterizer;
        processPipelineInfo.pMultisampleState = &multisampling;
        processPipelineInfo.pDepthStencilState = &depthStencil;
        processPipelineInfo.pColorBlendState = &colorBlending;
        processPipelineInfo.pDynamicState = &dynamicState;
        processPipelineInfo.layout = pipelineLayouts[3];
        processPipelineInfo.renderPass = swapchain->renderPass;
        processPipelineInfo.subpass = 3;
        processPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device.get()->device, VK_NULL_HANDLE, 1, &processPipelineInfo, nullptr, &graphicsPipelines[3]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device.get()->device, processFragShaderModule, nullptr);
        vkDestroyShaderModule(device.get()->device, processVertShaderModule, nullptr);

    }
    //helper

    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    VkShaderModule createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device.get()->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }
};

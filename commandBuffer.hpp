#pragma once
#include"Object.hpp"
#include"picture.hpp"
class CommandBuffer {
public:
    std::shared_ptr<Device> device;
    std::shared_ptr<SyncObj> sync;
    std::shared_ptr<Swap_Chain> swapchain;
    //std::vector<std::shared_ptr<InputBuffer>>inputs;

    std::shared_ptr<DescriptorSets>ufo;

    std::unique_ptr<PipeLine> g_pipeline;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t imageIndex;
    VkResult result;

    std::unique_ptr<Picture> pic;
	

    CommandBuffer(
        std::shared_ptr<Device> device, 
        std::shared_ptr<SyncObj> sync,
        std::shared_ptr<Swap_Chain> swapchain, 
        std::shared_ptr<DescriptorSets>ufo,
        std::unique_ptr<PipeLine> g_pipeline)
        : device{ device },sync { sync }, swapchain{ swapchain }, ufo{ ufo }
    {
        pic = std::make_unique<Picture>(device);
        this->g_pipeline = std::move(g_pipeline);
        createCommandBuffers();
    }
    
    void createCommandBuffers() {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device.get()->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(device->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
   
    void restCommandBuffer() {
        vkWaitForFences(device.get()->device, 1, &sync.get()->inFlightFences[swapchain.get()->currentFrame], VK_TRUE, UINT64_MAX);

        result = vkAcquireNextImageKHR(device->device, swapchain->swapChain, UINT64_MAX, sync->imageAvailableSemaphores[swapchain->currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            swapchain->recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(device->device, 1, &sync->inFlightFences[swapchain->currentFrame]);

        vkResetCommandBuffer(commandBuffers[swapchain->currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    }

    void submitCommand() {
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { sync.get()->imageAvailableSemaphores[swapchain.get()->currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[swapchain->currentFrame];

        VkSemaphore signalSemaphores[] = { sync->renderFinishedSemaphores[swapchain->currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(device.get()->graphicsQueue, 1, &submitInfo, sync->inFlightFences[swapchain->currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapchain->swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(device->presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            swapchain->recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        swapchain->currentFrame = (swapchain->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void drawFrame() {
        restCommandBuffer(); 
        recordCommandBuffer(commandBuffers[swapchain.get()->currentFrame], imageIndex);
        submitCommand();
    }

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain->renderPass;
        renderPassInfo.framebuffer = swapchain->swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapchain->swapChainExtent;

        std::array<VkClearValue, 8> clearValues{};
        clearValues[0].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };
        clearValues[2].depthStencil = { 1.0f,0 };
        clearValues[3].depthStencil = { 1.0f,0 };
        clearValues[4].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
        clearValues[5].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
        clearValues[6].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
        clearValues[7].color = { {1.0f, 1.0f, 1.0f, 1.0f} };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain->swapChainExtent.width;
        viewport.height = (float)swapchain->swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchain->swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        ufo->updateUniformBuffer(swapchain.get());

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline.get()->graphicsPipelines[0]);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline->pipelineLayouts[0], 0, 1, &ufo->descriptorSets[0][swapchain->currentFrame], 0, nullptr);

        
        for (auto &obj:objs) {
            
            pushConstant push{};
            push.modelTransform = obj.transform.getModelMatrix();
            push.normalTransform = obj.transform.getNormalMatrix();
            push.num = obj.textureindex;
            vkCmdPushConstants(
                commandBuffer,
                g_pipeline->pipelineLayouts[0],
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(pushConstant),
                &push
            );
         
                
            
           
            meshs.find(obj.mesh)->second->bind(commandBuffer, g_pipeline->pipelineLayouts[0], swapchain->currentFrame);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(meshs.find(obj.mesh)->second->indices.size()), 1, 0, 0, 0);
            
        }


        vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline.get()->graphicsPipelines[1]);//
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline->pipelineLayouts[1], 0, 1, &ufo->descriptorSets[1][swapchain->currentFrame], 0, nullptr);
        for (auto& obj : objs) {

            pushConstant push{};
            push.modelTransform = obj.transform.getModelMatrix();
            push.normalTransform = obj.transform.getNormalMatrix();
            push.num = obj.textureindex;

            vkCmdPushConstants(
                commandBuffer,
                g_pipeline->pipelineLayouts[1],
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(pushConstant),
                &push
            );

            meshs.find(obj.mesh)->second->bind(commandBuffer, g_pipeline->pipelineLayouts[1], swapchain->currentFrame);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(meshs.find(obj.mesh)->second->indices.size()), 1, 0, 0, 0);

        }

        vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline.get()->graphicsPipelines[2]);//
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline->pipelineLayouts[2], 0, 1, &ufo->descriptorSets[2][swapchain->currentFrame], 0, nullptr);
        for (auto& obj : objs) {

            pushConstant push{};
            push.modelTransform = obj.transform.getModelMatrix();
            push.normalTransform = obj.transform.getNormalMatrix();
            push.num = obj.textureindex;

            vkCmdPushConstants(
                commandBuffer,
                g_pipeline->pipelineLayouts[2],
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(pushConstant),
                &push
            );

            meshs.find(obj.mesh)->second->bind(commandBuffer, g_pipeline->pipelineLayouts[2], swapchain->currentFrame);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(meshs.find(obj.mesh)->second->indices.size()), 1, 0, 0, 0);

        }

        vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline.get()->graphicsPipelines[3]);//
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipeline->pipelineLayouts[3], 0, 1, &ufo->descriptorSets[3][swapchain->currentFrame], 0, nullptr);
        pic->bind(commandBuffer, g_pipeline->pipelineLayouts[3], swapchain->currentFrame);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(pic->indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
};
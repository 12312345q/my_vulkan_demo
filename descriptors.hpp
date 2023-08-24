#pragma once
#include"camera.hpp"
#include"uv.hpp"
#include<map>
#include"object.hpp"
struct Matrix {
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
};


struct pushConstant {
    alignas(16) glm::mat4 modelTransform;
    alignas(16) glm::mat4 normalTransform;
    int num;

};

struct Light {
    alignas(16)glm::vec3 position;
    alignas(16)glm::vec3 direction;
    alignas(16)glm::vec4 color;
    alignas(4)float range;
    alignas(4)float near;
    alignas(4)float far;
    alignas(4)float size;

};
class Layout {
public:
    std::shared_ptr<Device>device;
    std::array<VkDescriptorSetLayout,4> descriptorSetLayouts;
    Layout(std::shared_ptr<Device>device) :device{ device } {
        createDescriptorSetLayout();

    }
    ~Layout() {
        for (auto i : descriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(device.get()->device, i, nullptr);
        }
       
    }

    void createDescriptorSetLayout() {
        
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT| VK_SHADER_STAGE_FRAGMENT_BIT;


        VkDescriptorSetLayoutBinding samplerBinding{};
        samplerBinding.binding = 1;
        samplerBinding.descriptorCount = 1;
        samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerBinding.pImmutableSamplers = nullptr;
        samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


        VkDescriptorSetLayoutBinding imageBinding{};
        imageBinding.binding = 2;
        imageBinding.descriptorCount = MAX_TEXTURES_IN;
        imageBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        imageBinding.pImmutableSamplers = nullptr;
        imageBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding lightBinding{};
        lightBinding.binding = 3;
        lightBinding.descriptorCount = 1;
        lightBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightBinding.pImmutableSamplers = nullptr;
        lightBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT| VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding shadowDataBinding{};
        shadowDataBinding.binding = 4;
        shadowDataBinding.descriptorCount = 1;
        shadowDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        shadowDataBinding.pImmutableSamplers = nullptr;
        shadowDataBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        ;

        VkDescriptorSetLayoutBinding depthBinding{};
        depthBinding.binding = 5;
        depthBinding.descriptorCount = 1;
        depthBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        depthBinding.pImmutableSamplers = nullptr;
        depthBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding noiseBinding{};
        noiseBinding.binding = 6;
        noiseBinding.descriptorCount = 1;
        noiseBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        noiseBinding.pImmutableSamplers = nullptr;
        noiseBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding pictureBinding{};
        pictureBinding.binding = 7;
        pictureBinding.descriptorCount = 1;
        pictureBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        pictureBinding.pImmutableSamplers = nullptr;
        pictureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        
        VkDescriptorSetLayoutBinding positionBinding{};
        positionBinding.binding = 8;
        positionBinding.descriptorCount = 1;
        positionBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        positionBinding.pImmutableSamplers = nullptr;
        positionBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding normalBinding{};
        normalBinding.binding = 9;
        normalBinding.descriptorCount = 1;
        normalBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        normalBinding.pImmutableSamplers = nullptr;
        normalBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        

        //binding0
        std::array<VkDescriptorSetLayoutBinding, 1>shadowLayout = { lightBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo0{};
        layoutInfo0.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo0.bindingCount = shadowLayout.size();
        layoutInfo0.pBindings = shadowLayout.data();

        if (vkCreateDescriptorSetLayout(device.get()->device, &layoutInfo0, nullptr, &descriptorSetLayouts[0]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
        //binding1
        std::array<VkDescriptorSetLayoutBinding, 1>cameraLayout = { uboLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo1{};
        layoutInfo1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo1.bindingCount = cameraLayout.size();
        layoutInfo1.pBindings = cameraLayout.data();

        if (vkCreateDescriptorSetLayout(device.get()->device, &layoutInfo1, nullptr, &descriptorSetLayouts[1]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }


        //binding2
        std::array<VkDescriptorSetLayoutBinding, 7>prebindings = { uboLayoutBinding ,samplerBinding,imageBinding ,lightBinding,shadowDataBinding, depthBinding, noiseBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo2{};
        layoutInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo2.bindingCount = prebindings.size();
        layoutInfo2.pBindings = prebindings.data();

        if (vkCreateDescriptorSetLayout(device.get()->device, &layoutInfo2, nullptr, &descriptorSetLayouts[2]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        //binding3
        std::array<VkDescriptorSetLayoutBinding, 7>afterbindings = { uboLayoutBinding, samplerBinding, depthBinding, noiseBinding, pictureBinding ,positionBinding, normalBinding};

        VkDescriptorSetLayoutCreateInfo layoutInfo3{};
        layoutInfo3.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo3.bindingCount = afterbindings.size();
        layoutInfo3.pBindings = afterbindings.data();

        if (vkCreateDescriptorSetLayout(device.get()->device, &layoutInfo3, nullptr, &descriptorSetLayouts[3]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

};

class Descriptorpool {
public:
    std::shared_ptr<Device>device;
    VkDescriptorPool descriptorPool;

    Descriptorpool(std::shared_ptr<Device>device) :device{ device } {
        createDescriptorPool();
    }
    ~Descriptorpool() {
        vkDestroyDescriptorPool(device.get()->device, descriptorPool, nullptr);
    }

    void createDescriptorPool() {
        VkDescriptorPoolSize ufoSize{};
        ufoSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ufoSize.descriptorCount= 8;

        VkDescriptorPoolSize samplerSize{};
        samplerSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerSize.descriptorCount = 8;

        VkDescriptorPoolSize imageSize{};
        imageSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        imageSize.descriptorCount = 8;

        VkDescriptorPoolSize inputSize{};
        inputSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        inputSize.descriptorCount = 16;

        std::array<VkDescriptorPoolSize, 4> poolSizes = { ufoSize,samplerSize,imageSize ,inputSize };


        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 32;

        if (vkCreateDescriptorPool(device->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
};

class DescriptorSets {
public:
    std::shared_ptr<Device>device;
    std::shared_ptr<Sampler>sampler;
    std::shared_ptr<CameraController>camera;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    std::vector<VkBuffer> lightBuffers;
    std::vector<VkDeviceMemory> lightBuffersMemory;
    std::vector<void*> lightBuffersMapped;
    //noise texture
    std::shared_ptr<Texture> noise;

    std::vector<std::vector<VkDescriptorSet>> descriptorSets;

    DescriptorSets(
        std::shared_ptr<Device>device, 
        std::shared_ptr<Swap_Chain>swapchain,
        std::shared_ptr<Layout>layout, 
        std::shared_ptr<Descriptorpool>desp,
        std::shared_ptr<Sampler>sampler,
        std::shared_ptr<CameraController>camera) 
        :device{ device }, sampler{ sampler }, camera{ camera }{
        noise = std::make_shared <Texture>(device, NOISE_TEXTURE_PATH);

        createUniformBuffers();
        createLightBuffers();
        setLightData();
        createDescriptorSets(layout.get(),desp.get(),swapchain.get());
    }
    ~DescriptorSets() {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(device.get()->device, uniformBuffers[i], nullptr);
            vkFreeMemory(device.get()->device, uniformBuffersMemory[i], nullptr);

            vkDestroyBuffer(device.get()->device, lightBuffers[i], nullptr);
            vkFreeMemory(device.get()->device, lightBuffersMemory[i], nullptr);
        }

        
    }

    void createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(Matrix);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i], device.get());

            vkMapMemory(device->device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
        }
    }

    void createLightBuffers() {
        VkDeviceSize bufferSize = sizeof(Matrix);

        lightBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        lightBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        lightBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, lightBuffers[i], lightBuffersMemory[i], device.get());

            vkMapMemory(device->device, lightBuffersMemory[i], 0, bufferSize, 0, &lightBuffersMapped[i]);
        }
    }

    void setLightData() {
        Light light{};
        light.position = { 1.1f,2.0f,0.5f };
        light.direction = { -1.1f,-1.0f,-0.5f };
        light.color = { 1.f,1.f,1.f,0.8f };
        light.range = 2.6;
        light.near = 0.1;
        light.far = 5.1;
        light.size = 1.5;
        for (auto i = 0; i < lightBuffersMapped.size(); i++) {
            memcpy(lightBuffersMapped[i], &light, sizeof(light));
        }

    }


    void createDescriptorSets(Layout* descriptorSetLayout,Descriptorpool* despool,Swap_Chain*swapchain) {

        descriptorSets.resize(descriptorSetLayout->descriptorSetLayouts.size());
        for (auto i = 0; i < descriptorSetLayout->descriptorSetLayouts.size();i++) {
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout->descriptorSetLayouts[i]);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = despool->descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets[i].resize(MAX_FRAMES_IN_FLIGHT);
            if (vkAllocateDescriptorSets(device->device, &allocInfo, descriptorSets[i].data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }
           
        }
        

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(Matrix);
            

            VkDescriptorImageInfo samplerInfo = sampler->getInfo();
            

            VkDescriptorImageInfo infos[MAX_TEXTURES_IN]{};
            int exp = 0;
            for (auto& kv : textures) {
                VkDescriptorImageInfo imageInfo = kv.second->getInfo();
                infos[exp] = imageInfo;
                exp++;
            }
            

            VkDescriptorBufferInfo lightInfo{};
            lightInfo.buffer = lightBuffers[i];
            lightInfo.offset = 0;
            lightInfo.range = sizeof(Light);
            
            VkDescriptorImageInfo shadowInfo{};
            shadowInfo.imageView = swapchain->shadowImageView;
            shadowInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            shadowInfo.sampler = VK_NULL_HANDLE;
            
            VkDescriptorImageInfo depthInfo{};
            depthInfo.imageView = swapchain->cameraImageView;
            depthInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            depthInfo.sampler = VK_NULL_HANDLE;
            
            VkDescriptorImageInfo noiseInfo{};
            noiseInfo.imageView = noise->textureImageView;
            noiseInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            noiseInfo.sampler = VK_NULL_HANDLE;

            VkDescriptorImageInfo afterImageInfo{};
            afterImageInfo.imageView = swapchain->afterImageView;
            afterImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            afterImageInfo.sampler = VK_NULL_HANDLE;

            VkDescriptorImageInfo shadowDataInfo{};
            shadowDataInfo.imageView = swapchain->shadowDataImageView;
            shadowDataInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            shadowDataInfo.sampler = VK_NULL_HANDLE;

            VkDescriptorImageInfo positionInfo{};
            positionInfo.imageView = swapchain->positionTextureView;
            positionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            positionInfo.sampler = VK_NULL_HANDLE;

            VkDescriptorImageInfo normalInfo{};
            normalInfo.imageView = swapchain->normalTextureView;
            normalInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            normalInfo.sampler = VK_NULL_HANDLE;

            //write0
            VkWriteDescriptorSet lightWrite = {};
            lightWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            lightWrite.dstSet = descriptorSets[0][i];
            lightWrite.dstBinding = 3;
            lightWrite.dstArrayElement = 0;
            lightWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            lightWrite.descriptorCount = 1;
            lightWrite.pBufferInfo = &lightInfo;

            std::array<VkWriteDescriptorSet, 1> shadowWrites = { lightWrite };

            vkUpdateDescriptorSets(device->device, shadowWrites.size(), shadowWrites.data(), 0, nullptr);
            //write1
            VkWriteDescriptorSet cameraUfoWrite = {};
            cameraUfoWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            cameraUfoWrite.dstSet = descriptorSets[1][i];
            cameraUfoWrite.dstBinding = 0;
            cameraUfoWrite.dstArrayElement = 0;
            cameraUfoWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            cameraUfoWrite.descriptorCount = 1;
            cameraUfoWrite.pBufferInfo = &bufferInfo;

            std::array<VkWriteDescriptorSet, 1> cameraWrites = { cameraUfoWrite };

            vkUpdateDescriptorSets(device->device, cameraWrites.size(), cameraWrites.data(), 0, nullptr);
            
            //write2
            VkWriteDescriptorSet ufoWrite = {};
            ufoWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            ufoWrite.dstSet = descriptorSets[2][i];
            ufoWrite.dstBinding = 0;
            ufoWrite.dstArrayElement = 0;
            ufoWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ufoWrite.descriptorCount = 1;
            ufoWrite.pBufferInfo = &bufferInfo;

            VkWriteDescriptorSet samplerWrite = {};
            samplerWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            samplerWrite.dstSet = descriptorSets[2][i];
            samplerWrite.dstBinding = 1;
            samplerWrite.dstArrayElement = 0;
            samplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            samplerWrite.descriptorCount = 1;
            samplerWrite.pImageInfo = &samplerInfo;

            VkWriteDescriptorSet imageWriter = {};
            imageWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            imageWriter.dstSet = descriptorSets[2][i];
            imageWriter.dstBinding = 2;
            imageWriter.dstArrayElement = 0;
            imageWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            imageWriter.descriptorCount = MAX_TEXTURES_IN;
            imageWriter.pImageInfo = infos;

            VkWriteDescriptorSet lightWriteNext = {};
            lightWriteNext.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            lightWriteNext.dstSet = descriptorSets[2][i];
            lightWriteNext.dstBinding = 3;
            lightWriteNext.dstArrayElement = 0;
            lightWriteNext.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            lightWriteNext.descriptorCount = 1;
            lightWriteNext.pBufferInfo = &lightInfo;

            VkWriteDescriptorSet shadowDataWriter = {};
            shadowDataWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            shadowDataWriter.dstSet = descriptorSets[2][i];
            shadowDataWriter.dstBinding = 4;
            shadowDataWriter.dstArrayElement = 0;
            shadowDataWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            shadowDataWriter.descriptorCount = 1;
            shadowDataWriter.pImageInfo = &shadowDataInfo;

            VkWriteDescriptorSet depthWriter = {};
            depthWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            depthWriter.dstSet = descriptorSets[2][i];
            depthWriter.dstBinding = 5;
            depthWriter.dstArrayElement = 0;
            depthWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            depthWriter.descriptorCount = 1;
            depthWriter.pImageInfo = &depthInfo;

            VkWriteDescriptorSet noiseWriter = {};
            noiseWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            noiseWriter.dstSet = descriptorSets[2][i];
            noiseWriter.dstBinding = 6;
            noiseWriter.dstArrayElement = 0;
            noiseWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            noiseWriter.descriptorCount = 1;
            noiseWriter.pImageInfo = &noiseInfo;

            

            std::array<VkWriteDescriptorSet, 7> renderWrites = { ufoWrite ,samplerWrite ,imageWriter ,lightWriteNext ,shadowDataWriter ,depthWriter, noiseWriter };

            vkUpdateDescriptorSets(device->device, static_cast<uint32_t>(renderWrites.size()), renderWrites.data(), 0, nullptr);

            //write3
            VkWriteDescriptorSet anotherUfoWrite = {};
            anotherUfoWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            anotherUfoWrite.dstSet = descriptorSets[3][i];
            anotherUfoWrite.dstBinding = 0;
            anotherUfoWrite.dstArrayElement = 0;
            anotherUfoWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            anotherUfoWrite.descriptorCount = 1;
            anotherUfoWrite.pBufferInfo = &bufferInfo;

            VkWriteDescriptorSet anotherSamplerWrite = {};
            anotherSamplerWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            anotherSamplerWrite.dstSet = descriptorSets[3][i];
            anotherSamplerWrite.dstBinding = 1;
            anotherSamplerWrite.dstArrayElement = 0;
            anotherSamplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            anotherSamplerWrite.descriptorCount = 1;
            anotherSamplerWrite.pImageInfo = &samplerInfo;

            VkWriteDescriptorSet anotherDepthWriter = {};
            anotherDepthWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            anotherDepthWriter.dstSet = descriptorSets[3][i];
            anotherDepthWriter.dstBinding = 5;
            anotherDepthWriter.dstArrayElement = 0;
            anotherDepthWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            anotherDepthWriter.descriptorCount = 1;
            anotherDepthWriter.pImageInfo = &depthInfo;

            VkWriteDescriptorSet anotherNoiseWriter = {};
            anotherNoiseWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            anotherNoiseWriter.dstSet = descriptorSets[3][i];
            anotherNoiseWriter.dstBinding = 6;
            anotherNoiseWriter.dstArrayElement = 0;
            anotherNoiseWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            anotherNoiseWriter.descriptorCount = 1;
            anotherNoiseWriter.pImageInfo = &noiseInfo;

            VkWriteDescriptorSet afterImageWriter = {};
            afterImageWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            afterImageWriter.dstSet = descriptorSets[3][i];
            afterImageWriter.dstBinding = 7;
            afterImageWriter.dstArrayElement = 0;
            afterImageWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            afterImageWriter.descriptorCount = 1;
            afterImageWriter.pImageInfo = &afterImageInfo;
            
            VkWriteDescriptorSet positionWriter = {};
            positionWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            positionWriter.dstSet = descriptorSets[3][i];
            positionWriter.dstBinding = 8;
            positionWriter.dstArrayElement = 0;
            positionWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            positionWriter.descriptorCount = 1;
            positionWriter.pImageInfo = &positionInfo;

            VkWriteDescriptorSet normalWriter = {};
            normalWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            normalWriter.dstSet = descriptorSets[3][i];
            normalWriter.dstBinding = 9;
            normalWriter.dstArrayElement = 0;
            normalWriter.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            normalWriter.descriptorCount = 1;
            normalWriter.pImageInfo = &normalInfo;
            
            std::array<VkWriteDescriptorSet, 7> afterDealWrites = { anotherUfoWrite, anotherSamplerWrite, anotherDepthWriter, anotherNoiseWriter, afterImageWriter, positionWriter, normalWriter};

            vkUpdateDescriptorSets(device->device, static_cast<uint32_t>(afterDealWrites.size()), afterDealWrites.data(), 0, nullptr);

        }
    }

    
  
    void updateUniformBuffer(Swap_Chain* swapchain) {
   
        Matrix ubo{};
        ubo.proj = camera->proj;
        ubo.position = camera->position;
        ubo.direction = camera->direction;
;

        memcpy(uniformBuffersMapped[swapchain->currentFrame], &ubo, sizeof(ubo));
    }

};
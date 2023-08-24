#pragma once
#include"createBufferHelper.hpp"
#include"transform.hpp"
class Picture {
public:
	std::shared_ptr<Device>device;
	Transform transform;
	std::vector<glm::vec3>vertices = {
		glm::vec3(-1.0,-1.0,0.0),
		glm::vec3(-1.0,1.0,0.0),
		glm::vec3(1.0,1.0,0.0),
		glm::vec3(1.0,-1.0,0.0),

	};
	std::vector<uint32_t>indices = { 0,1,2,2,3,0 };

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	Picture() {};

	Picture(std::shared_ptr<Device>device) :device{ device } {
		createVertexBuffer();
		createIndexBuffer();
	};
	~Picture() {
		vkDestroyBuffer(device.get()->device, indexBuffer, nullptr);
		vkFreeMemory(device.get()->device, indexBufferMemory, nullptr);

		vkDestroyBuffer(device.get()->device, vertexBuffer, nullptr);
		vkFreeMemory(device.get()->device, vertexBufferMemory, nullptr);
	};



	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(glm::vec3);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset =0;

		return attributeDescriptions;
	}

	void createVertexBuffer() {
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device.get());

		void* data;
		vkMapMemory(device->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device->device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, device.get());

		copyBuffer(stagingBuffer, vertexBuffer, bufferSize, device.get());

		vkDestroyBuffer(device->device, stagingBuffer, nullptr);
		vkFreeMemory(device->device, stagingBufferMemory, nullptr);
	}
	//
	void createIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device.get());

		void* data;
		vkMapMemory(device->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device->device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory, device.get());

		copyBuffer(stagingBuffer, indexBuffer, bufferSize, device.get());

		vkDestroyBuffer(device->device, stagingBuffer, nullptr);
		vkFreeMemory(device->device, stagingBufferMemory, nullptr);
	}

	void bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t frameIndex) {

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	}
	
};

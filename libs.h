#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
};
const int MAX_TEXTURES_IN = 1;//值为使用的纹理总数

const int MAX_FRAMES_IN_FLIGHT = 2;

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

const char* SHADOW_VERTEX_SHADER_PATH = "shaders/shadowV.spv";
const char* SHADOW_FRAGMENT_SHADER_PATH = "shaders/shadowF.spv";

const char* CAMERA_VERTEX_SHADER_PATH = "shaders/depthV.spv";
const char* CAMERA_FRAGMENT_SHADER_PATH = "shaders/depthF.spv";

const char* LIGHT_VERTEX_SHADER_PATH = "shaders/vert.spv";
const char* LIGHT_FRAGMENT_SHADER_PATH = "shaders/frag.spv";

const char* PROCESSING_VERTEX_SHADER_PATH = "shaders/processV.spv";
const char* PROCESSING_FRAGMENT_SHADER_PATH = "shaders/processF.spv";


const char* NOISE_TEXTURE_PATH = "textures/noise.jpg";


const char* BOX_LEFT_PATH = "";
const char* BOX_RIGHT_PATH = "";
const char* BOX_TOP_PATH = "";
const char* BOX_BUTTOM_PATH = "";
const char* BOX_FORWARD_PATH = "";
const char* BOX_BACK_PATH = "";

const char* BOX_PATH = "";
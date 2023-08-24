#include"libs.h"
#include"window.hpp"
#include"device.hpp"
#include"swap_chain.hpp"
#include"pipeline.hpp"
#include"sync.hpp"
#include"commandBuffer.hpp"
#include"object.hpp"
#include"transform.hpp"
#include"camera.hpp"
#include"descriptors.hpp"
#include"picture.hpp"
#include"skybox.hpp"
class App {
public:
    App() {
        window = std::make_shared<Window>();
        device = std::make_shared<Device>(window);
        player = std::make_shared<CameraController>(glm::vec3{ 1.1f,1.5f,1.0f }, glm::vec3(-1.1f, -1.0f, -1.0f));
        player->setPerspectiveProjection(glm::radians(40.f), (float)WIDTH / HEIGHT, .1f, 50.f);
        
        //Object o3 = Object(device, "models//sphere.obj", "textures/aaa.jpg");
        //o3.transform.setScale(0.02, 0.12, 0.02);
        //o3.transform.setTranslation(0.4, 0.0, 0.0);
        //o3.ready();
        
        
        Object o1 = Object(device, "models//bunny.obj", "textures/white.jpg");
        o1.transform.setScale(0.3, 0.3, 0.3);
        o1.transform.setTranslation(-0.4, 0.0, 0.0);
        o1.ready();
        
        Object o2 = Object(device, "models//flat.obj", "textures/white.jpg");
        o2.transform.setScale(2.0, 2.0, 2.0);
        o2.transform.setTranslation(0.0, 0.0, 0.0);
        o2.ready();

        sampler = std::make_shared<Sampler>(device);
        despool = std::make_shared<Descriptorpool>(device);
        layout = std::make_shared<Layout>(device);
        my_swapChain = std::make_shared<Swap_Chain>(device);
        sync = std::make_shared<SyncObj>(device);
        descriptorsets = std::make_shared<DescriptorSets>(device, my_swapChain,layout,despool,sampler,player);
        graphicsPipeline = std::make_unique<PipeLine>(device,my_swapChain, layout);
        cmdBuffer = std::make_unique<CommandBuffer>(
            device,
            sync,
            my_swapChain,
            descriptorsets,
            std::move(graphicsPipeline)
        );
            
    }
   
    void run() {


        while (!glfwWindowShouldClose(window.get()->window)) {
            
            glfwPollEvents();

            player->keyboardListener(window->window);
            player->mouseListener(window->window);
            
            cmdBuffer->drawFrame();


        }

        vkDeviceWaitIdle(device->device); 
        
    }

private:
    std::shared_ptr<CameraController>player;
    std::shared_ptr<Window>window;
    std::shared_ptr<Device> device;
    std::shared_ptr<Descriptorpool>despool;
    std::shared_ptr<Layout>layout;
    std::shared_ptr<Sampler>sampler;
    std::shared_ptr<DescriptorSets>descriptorsets;
    std::shared_ptr<Swap_Chain> my_swapChain;
    std::shared_ptr<SyncObj> sync;
    std::unique_ptr<PipeLine> graphicsPipeline;
    std::unique_ptr<CommandBuffer> cmdBuffer;
};

  
int main() {
    App app;
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
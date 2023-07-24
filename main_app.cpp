#include "main_app.h"
#include "simple_render_system.h"
#include "key_move_controller.h"

// 3rd party
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

// std
#include <chrono>
#include <stdexcept>
#include <array>

namespace oeg
{

    struct alignas(16) GlobalUbo {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection{normalize(glm::vec3(1.0f, -3.0f, -1.0f))};
    };

    OegEngine::OegEngine() { loadGameObjects(); }

	OegEngine::~OegEngine()
	= default;

	void OegEngine::run() 
	{
        OegBuffer globalUboBuffer{
		oegDevice,
		sizeof(GlobalUbo),
		OegSwapChain::MAX_FRAMES_IN_FLIGHT,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        	oegDevice.properties.limits.minUniformBufferOffsetAlignment,
        };

        globalUboBuffer.map();

		SimpleRenderSystem simpleRenderSystem{ oegDevice, oegRenderer.getSwapChainRenderPass() };
        OegCamera camera{};
        // camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

        auto viewerObject = OegGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();


		while (!oegWindow.shouldClose()) 
		{

			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;
            cameraController.moveInPlaneXZ(oegWindow.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = oegRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.0f);
			if (auto commandBuffer = oegRenderer.beginFrame())
			{
                int frameIndex = oegRenderer.getFrameIndex();

                //  update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                globalUboBuffer.writeToIndex(&ubo, frameIndex);
                globalUboBuffer.flushIndex(frameIndex);

                //  render
				oegRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects,camera);
				oegRenderer.endSwapChainRenderPass(commandBuffer);
				oegRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(oegDevice.device());
	}

	void OegEngine::loadGameObjects()
	{
        std::shared_ptr<OegModel> oegModel = OegModel::createModelFromFile(oegDevice, "models/mewlin_orgestedit_v3.obj");
        auto cube = OegGameObject::createGameObject();
        cube.model = oegModel;
        cube.transform.translation = { 0.0f, 0.0f, 2.5f };
        cube.transform.scale = { 0.5f, 0.5f, 0.5f };

        gameObjects.push_back(std::move(cube));

	}
}
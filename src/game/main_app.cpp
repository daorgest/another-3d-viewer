#include "key_move_controller.h"
#include "main_app.h"
#include "simple_render_system.h"

// 3rd party
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

// std
#include <chrono>

namespace oeg
{
	struct alignas(16) GlobalUbo
	{
		glm::mat4 projectionView{1.0f};
		glm::vec3 lightDirection = normalize(glm::vec3(1.0f, -3.0f, -1.0f));
	};

	OegEngine::OegEngine()
	{
		loadGameObjects();
		initImGui();
		setupRenderSystem();
	}

	OegEngine::~OegEngine() = default;

	void OegEngine::initImGui()
	{
		oegWindow.initImGui(
			oegDevice.getInstance(),
			oegDevice.getPhysicalDevice(),
			oegDevice.device(),
			oegDevice.findPhysicalQueueFamilies().graphicsFamily,
			oegDevice.graphicsQueue(),
			oegRenderer.getSwapChainRenderPass(),
			OegSwapChain::MAX_FRAMES_IN_FLIGHT
		);
	}

	void OegEngine::run()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto viewerObject = OegGameObject::createGameObject();

		float fov = 70.0f; // Initial FOV value

		float updateInterval = 0.5f; // Update ImGui every 0.5 seconds
		float timeSinceLastUpdate = 0.0f;

		int frameCount = 0;
		float totalFrameTime = 0.0f;

		constexpr int maxFrameSamples = 100;
		std::vector<float> frameTimes(maxFrameSamples, 0.0f);

		while (!oegWindow.shouldClose())
		{
			KeyboardMovementController cameraController;
			glfwPollEvents();

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();


			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
			currentTime = newTime;

			timeSinceLastUpdate += frameTime;

			totalFrameTime += frameTime;
			frameCount++;

			// Store the frame time in the circular buffer
			frameTimes[frameCount % maxFrameSamples] = frameTime;


			float frameTimeMs = frameTime * 1000.0f;
			ImGui::Text("Frame Time: %.2f ms", frameTimeMs);

			float fps = frameCount / totalFrameTime;
			ImGui::Text("FPS: %.2f", fps);

			// FOV slider
			ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f);
			ImGui::SliderFloat("Camera Speed", &cameraController.moveSpeed, 3.0f, 6.0f);

			timeSinceLastUpdate = 0.0f; // Reset the timer
			frameCount = 0;
			totalFrameTime = 0.0f;

			cameraController.moveInPlaneXZ(oegWindow.getGLFWWindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
			camera.setPerspectiveProjection(glm::radians(fov), oegRenderer.getAspectRatio(), 0.1f, 10.0f);

			renderFrame(frameTime);
		}

		vkDeviceWaitIdle(oegDevice.device());
	}


	void OegEngine::renderFrame(float frameTime)
	{
		if (auto commandBuffer = oegRenderer.beginFrame())
		{
			ImGui::Render();
			int frameIndex = oegRenderer.getFrameIndex();

			updateGlobalUbo(frameIndex, frameTime);
			oegRenderer.beginSwapChainRenderPass(commandBuffer);

			FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};
			simpleRenderSystem->renderGameObjects(frameInfo, gameObjects);

			if (ImDrawData* drawData = ImGui::GetDrawData())
			{
				ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
			}
			oegRenderer.endSwapChainRenderPass(commandBuffer);
			oegRenderer.endFrame();
		}

		ImGui::Render();
	}

	void OegEngine::updateGlobalUbo(const int frameIndex, float frameTime)
	{
		GlobalUbo ubo{};
		ubo.projectionView = camera.getProjection() * camera.getView();

		globalUboBuffer->writeToIndex(&ubo, frameIndex);
		globalUboBuffer->flushIndex(frameIndex);
	}

	void OegEngine::loadGameObjects()
	{
		auto oegModel = OegModel::createModelFromFile(oegDevice, "models/test.obj");
		OegGameObject cube = OegGameObject::createGameObject();

		// Convert unique_ptr to shared_ptr
		cube.model = std::shared_ptr(std::move(oegModel));
		cube.transform.translation = glm::vec3(0.0f, 0.0f, 2.5f);
		cube.transform.scale = glm::vec3(0.5f, 0.5f, 0.5f);

		gameObjects.push_back(std::move(cube));
	}


	void OegEngine::setupRenderSystem()
	{
		globalUboBuffer = std::make_unique<OegBuffer>(
			oegDevice, // OegDevice&
			sizeof(GlobalUbo), // VkDeviceSize (instanceSize)
			OegSwapChain::MAX_FRAMES_IN_FLIGHT, // uint32_t (instanceCount)
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, // VkBufferUsageFlags (usageFlags)
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, // VkMemoryPropertyFlags (memoryPropertyFlags)
			oegDevice.getAllocator(), // VmaAllocator
			oegDevice.properties.limits.minUniformBufferOffsetAlignment // VkDeviceSize (minOffsetAlignment)
		);
		globalUboBuffer->map();
		simpleRenderSystem = std::make_unique<SimpleRenderSystem>(oegDevice, oegRenderer.getSwapChainRenderPass());
	}
} // namespace oeg

#pragma once

#include "../engine/oeg_camera.h"
#include "../engine/oeg_device.h"
#include "../engine/oeg_window.h"
#include "../engine/oeg_renderer.h"
#include "../engine/oeg_game_object.h"
#include "../engine/oeg_buffer.h"
#include "simple_render_system.h"
#include "key_move_controller.h"

#include <chrono>
#include <memory>
#include <vector>

namespace oeg
{
	class OegEngine
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		OegEngine();

		~OegEngine();

		OegEngine(const OegEngine&) = delete;

		OegEngine& operator=(const OegEngine&) = delete;

		void run();

	private:
		void initImGui();

		void setupRenderSystem();

		void mainLoop();

		void renderFrame(float frameTime);

		void updateGlobalUbo(int frameIndex, float frameTime);

		void loadGameObjects();

		void updateDeltaTime(std::chrono::time_point<std::chrono::high_resolution_clock>& currentTime);

		void updateCamera(OegGameObject& viewerObject, KeyboardMovementController& cameraController);

		OegWindow oegWindow{WIDTH, HEIGHT, "Vulkan App"};
		OegDevice oegDevice{oegWindow};
		OegRenderer oegRenderer{oegWindow, oegDevice};
		std::unique_ptr<OegBuffer> globalUboBuffer;
		std::unique_ptr<SimpleRenderSystem> simpleRenderSystem;
		std::vector<OegGameObject> gameObjects;
		OegCamera camera;
	};
} // namespace oeg

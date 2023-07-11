#pragma once

// my shit
#include "oeg_camera.h"
#include "oeg_device.h"
#include "oeg_window.h"
#include "oeg_renderer.h"
#include "oeg_game_object.h"

// std
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
		void loadGameObjects();

		OegWindow oegWindow{ WIDTH, HEIGHT, "Vulkan App" };
		OegDevice oegDevice{ oegWindow };
		OegRenderer oegRenderer{ oegWindow, oegDevice };

		std::vector<OegGameObject> gameObjects;
	};
}
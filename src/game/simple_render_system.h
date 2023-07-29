#pragma once

// my shit
#include "../engine/oeg_camera.h"
#include "../engine/oeg_device.h"
#include "../engine/oeg_game_object.h"
#include "../engine/oeg_pipeline.h"
#include "../engine/oeg_frame_info.h"


// std
#include <memory>
#include <vector>

namespace oeg
{
	class SimpleRenderSystem
	{
	public:

		SimpleRenderSystem(OegDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		void renderGameObjects(FrameInfo &frameInfo, std::vector<OegGameObject> &gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		
		OegDevice& oegDevice;

		std::unique_ptr<OegPipeline> oegPipeline;
		VkPipelineLayout pipelineLayout;
	};
}
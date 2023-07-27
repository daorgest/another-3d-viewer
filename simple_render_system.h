#pragma once

// my shit
#include "oeg_camera.h"
#include "oeg_device.h"
#include "oeg_game_object.h"
#include "oeg_pipeline.h"
#include "oeg_frame_info.h"


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
#pragma once

// my shit
#include "oeg_device.h"
#include "oeg_window.h"
#include "oeg_swap_chain.h"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace oeg
{
	class OegRenderer
	{
	public:


		OegRenderer(OegWindow& window, OegDevice& device);
		~OegRenderer();

		OegRenderer(const OegRenderer&) = delete;
		OegRenderer& operator=(const OegRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return oegSwapChain->getRenderPass(); }
		float getAspectRatio() const { return oegSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const
		{
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffer();
		void freeCommandBuffers();
		void recreateSwapChain();

		OegWindow& oegWindow;
		OegDevice& oegDevice;
		std::unique_ptr<OegSwapChain> oegSwapChain; // updating the swapchain with a new width and height with unique_ptr
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0}; // ........ just had to initialize it....
		bool isFrameStarted{false};
	};
}
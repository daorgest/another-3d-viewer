#include "oeg_renderer.h"

// std
#include <stdexcept>
#include <array>

namespace oeg
{
	OegRenderer::OegRenderer(OegWindow& window, OegDevice& device) 
		: oegWindow{window}, oegDevice{device}
	{
		recreateSwapChain();
		createCommandBuffer();
	}

	OegRenderer::~OegRenderer()
	{
		freeCommandBuffers();
	}

	void OegRenderer::recreateSwapChain()
	{
		auto extent = oegWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = oegWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(oegDevice.device());

		if (oegSwapChain == nullptr)
		{
			oegSwapChain = std::make_unique<OegSwapChain>(oegDevice, extent);
		} else {
			std::shared_ptr<OegSwapChain> oldSwapChain = std::move(oegSwapChain);
			oegSwapChain = std::make_unique<OegSwapChain>(oegDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*oegSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}

		// coming back to this
	}

	void OegRenderer::createCommandBuffer() 
	{
		commandBuffers.resize(OegSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = oegDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(oegDevice.device(), &allocInfo, commandBuffers.data())
			!= VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers! :((");
		}

	}

	void OegRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			oegDevice.device(),
			oegDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer OegRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Cant call beginFrame while already in progress!");

		auto result = oegSwapChain->acquireNextImage(&currentImageIndex);


		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to aquire next swapchain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer! :(");
		}
		return commandBuffer;
	}

	void OegRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call endFrame while frame is in progress...");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!!!");
		}

		auto result = oegSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || oegWindow.wasWindowResized())
		{
			oegWindow.resetWindowResiedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to aquire next swapchain image with command buffers!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % OegSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void OegRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is in progress...");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = oegSwapChain->getRenderPass();
		renderPassInfo.framebuffer = oegSwapChain->getFrameBuffer(currentImageIndex);

		// shader loading and storing
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = oegSwapChain->getSwapChainExtent();

		// clear values

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		//clearValues[1].depthStencil = ? ; VkClearValue is a union, so color and depth stencil is on the same memory address
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(oegSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(oegSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, oegSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void OegRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is in progress...");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}
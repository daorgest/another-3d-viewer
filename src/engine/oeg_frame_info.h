#pragma once

#include "oeg_camera.h"

//	lib

#include <vulkan/vulkan.h>

namespace oeg
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		OegCamera& camera;
	};
}
